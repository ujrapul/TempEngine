// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Engine.hpp"
#include "Audio/AudioSystem.hpp"
#include "Camera.hpp"
#include "Event.hpp"
#include "Luable.hpp"
#include "OpenGLWrapper.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "ThreadPool.hpp"
#include "Shader.hpp"
#include <atomic>
#include <chrono>
#include <filesystem>
#include <iterator>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#ifdef __linux__
#include "X11Render.hpp"
#elif __APPLE__
#include "NSRender.hpp"
#elif _WIN32
#include "WinRender.hpp"
#endif

namespace Temp
{
  namespace
  {
    // Exists only because I'm lazy
    Scene::Data* scene{nullptr};
    AudioSystem::Data audioSystem;
    std::vector<std::tuple<SceneObject::Data, std::function<void()>>> spawnObjects{};
    std::vector<SceneObject::Data> removeObjects{};
    std::vector<std::string> audioPaths{};
    float deltaTime{};
    float time{};
#ifdef DEBUG
    std::thread hotReloadThread{};
    std::unordered_set<int> shadersToReload{};
    std::atomic<bool> stopHotReloadThread{false};
    std::atomic<bool> reload{false};
#endif

#ifdef DEBUG
    void HotReloadThread()
    {
      std::vector<std::tuple<std::filesystem::path, std::filesystem::file_time_type>> shaders{};
      ThreadPool::Data threadPool{};
      ThreadPool::Initialize(threadPool);

      for (const auto& entry : std::filesystem::directory_iterator(AssetsDirectory() / "Shaders"))
      {
        if (!entry.exists())
        {
          continue;
        }
        shaders.push_back(std::make_tuple(entry, std::filesystem::last_write_time(entry)));
      }

      static auto shaderF =
        [](std::tuple<std::filesystem::path, std::filesystem::file_time_type>& shader) {
          using namespace Render;
          static std::mutex mtx{};
          auto shaderTime = std::filesystem::last_write_time(std::get<0>(shader));
          if (shaderTime != std::get<1>(shader))
          {
            std::get<1>(shader) = shaderTime;
            auto it = std::find(GlobalShaderFiles().begin(),
                                GlobalShaderFiles().end(),
                                std::get<0>(shader));
            if (it != GlobalShaderFiles().end())
            {
              std::lock_guard<std::mutex> lock(mtx);
              for (int i = 0; i < ShaderIdx::MAX; ++i)
              {
                shadersToReload.insert(i);
              }
            }
            else
            {
              size_t pos = std::find(ShaderFiles().begin(),
                                     ShaderFiles().end(),
                                     std::get<0>(shader).filename().string()) -
                           ShaderFiles().begin();
              if (pos < ShaderFiles().size())
              {
                std::lock_guard<std::mutex> lock(mtx);
                shadersToReload.insert((int)pos);
              }
            }
            reload = true;
          }
        };

      while (!stopHotReloadThread)
      {
        if (!reload)
        {
          ThreadPool::EnqueueForEach(threadPool, shaders, shaderF);
          ThreadPool::Wait(threadPool);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }

      ThreadPool::Destruct(threadPool);
    }
#endif

#ifdef __linux__
    void RunLinux(const char* windowName, int windowX, int windowY)
    {
      Global::Start(windowName, windowX, windowY);

      while (Global::IsActive())
      {
        static auto start = std::chrono::high_resolution_clock::now();
        auto stop = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(stop - start).count();
        start = stop;

        Global::Process(deltaTime);
      }
      Global::Destroy();
    }
#endif
  }

  Engine::Data Global::engine = {};

  void Global::Start(const char* windowName, int windowX, int windowY)
  {
    SceneObject::Init();
    Scene::Initialize(engine.scene);
    AudioSystem::ReadAudioFiles(audioSystem, audioPaths);

    scene = &engine.scene;
    scene->sceneFns = engine.sceneFns.front();
    // Start Render Thread
    Render::Initialize(*scene, windowName, windowX, windowY);

#ifdef DEBUG
    hotReloadThread = std::thread(HotReloadThread);
#endif
  }

  void Global::Process(float _deltaTime)
  {
    static Scene::SceneFns* nextSceneFns{nullptr};

    Temp::deltaTime = _deltaTime;
    time += _deltaTime;

    switch (scene->state)
    {
      case Scene::State::ENTER:
      {
        if (nextSceneFns)
        {
          scene->sceneFns = *nextSceneFns;
          nextSceneFns = nullptr;
        }
        Scene::ClearRender(*scene);
        // This should always run before DrawConstruct
        Scene::Construct(*scene);
        Scene::DrawConstruct(*scene);
        scene->state = Scene::State::RUN;
      }
      break;
      case Scene::State::RUN:
      {
        Scene::Update(*scene, deltaTime);
        Scene::DrawUpdate(*scene);
      }
      break;
      case Scene::State::LEAVE:
      {
        Scene::ClearRender(*scene);
        scene->state = Scene::State::ENTER;
        Scene::DrawDestruct(*scene);
        Scene::Destruct(*scene);
        nextSceneFns = scene->sceneFns.nextScene;
      }
      break;
      default:
        break;
    }

#ifdef DEBUG
    // Hot reload shaders
    if (reload)
    {
      if (shadersToReload.size() > 0)
      {
        Render::OpenGLWrapper::LoadShaders();
        for (auto shaderIdx : shadersToReload)
        {
          Scene::DrawReload(*scene, shaderIdx);
        }
        shadersToReload.clear();
      }
      reload = false;
    }
#endif

    // Process events in the renderer
    Render::Run(*scene);
    Input::Process(engine.inputData);

    if (spawnObjects.size() > 0)
    {
      Scene::SpawnObject(*scene, std::get<0>(spawnObjects.back()));
      std::get<1>(spawnObjects.back())();
      spawnObjects.pop_back();
    }

    if (removeObjects.size() > 0)
    {
      Scene::RemoveObject(*scene, removeObjects.back());
      removeObjects.pop_back();
    }
  }

  void Global::Run(const char* windowName, int windowX, int windowY)
  {
#ifdef __linux__
    RunLinux(windowName, windowX, windowY);
#elif __APPLE__
    Render::Run(engine.scene, windowName, windowX, windowY);
#elif _WIN32
    Global::Start(windowName, windowX, windowY);

    while (Global::IsActive())
    {
      static auto start = std::chrono::high_resolution_clock::now();
      auto stop = std::chrono::high_resolution_clock::now();
      float _deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(stop - start).count();
      start = stop;

      Global::Process(_deltaTime);
    }
    Global::Destroy();
    // Render::Run(engine.scene, windowName, windowX, windowY);
#endif
  }

  void Global::Destroy()
  {
    if (engine.scene.state == Scene::State::RUN)
    {
      Scene::ClearRender(*scene);
      Scene::DrawDestruct(*scene);
      Scene::Destruct(*scene);
    }
#ifdef DEBUG
    stopHotReloadThread = true;
    hotReloadThread.join();
#endif
    Render::Destroy();
    // For now the games should handle clean up of scenes
    Render::OpenGLWrapper::ClearShaderStrings();
    Scene::Destroy(engine.scene);
    AudioSystem::Destruct(audioSystem);
  }

  void Global::Construct(const Engine::Data& _engine) { Global::engine = _engine; }

  void Global::Quit() { engine.quit = true; }

  bool Global::IsActive() { return !engine.quit; }

  bool Global::IsSpawning() { return spawnObjects.size() > 0; }

  float Global::DeltaTime() { return deltaTime; }

  float Global::Time() { return time; }

  void Global::SetAudioPaths(const std::vector<std::string>& _audioPaths)
  {
    Temp::audioPaths = _audioPaths;
  }

  void Global::PlayAudioLoop(int i, std::atomic<bool>& stopHandle)
  {
    AudioSystem::PlayAudioLoop(audioSystem, i, stopHandle);
  }

  void Global::PlayAudio(int i)
  {
    static std::atomic_bool stop;
    AudioSystem::PlayAudio(audioSystem, i, stop);
  }

  void Global::ChangeMasterVolume(float volume)
  {
    AudioSystem::ChangeMasterVolume(volume);
  }

  void Global::ChangeVolume(int i, float volume)
  {
    AudioSystem::ChangeVolume(audioSystem, i, volume);
  }

  void Global::AddPressCallback(void (*callback)(Input::KeyboardCode), Input::KeyboardCode keyCode)
  {
    Input::AddPressCallback(callback, engine.inputData, keyCode);
  }

  void Global::RemovePressCallback(void (*callback)(Input::KeyboardCode),
                                   Input::KeyboardCode keyCode)
  {
    Input::RemovePressCallback(callback, engine.inputData, keyCode);
  }

  void Global::AddReleaseCallback(void (*callback)(Input::KeyboardCode),
                                  Input::KeyboardCode keyCode)
  {
    Input::AddReleaseCallback(callback, engine.inputData, keyCode);
  }

  void Global::RemoveReleaseCallback(void (*callback)(Input::KeyboardCode),
                                     Input::KeyboardCode keyCode)
  {
    Input::RemoveReleaseCallback(callback, engine.inputData, keyCode);
  }

  void Global::PushPressKeyQueue(Input::KeyboardCode keyCode) { Input::PushPressKeyQueue(keyCode); }

  void Global::PushReleaseKeyQueue(Input::KeyboardCode keyCode)
  {
    Input::PushReleaseKeyQueue(keyCode);
  }

  void Global::SpawnObject(const SceneObject::Data& object, std::function<void()> callback)
  {
    spawnObjects.push_back({object, std::move(callback)});
  }

  void Global::RemoveObject(const SceneObject::Data& object) { removeObjects.push_back(object); }
}
