// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Engine.hpp"
#include "Array_fwd.hpp"
#include "Audio/AudioSystem.hpp"
#include "Camera.hpp"
#include "EngineUtils.hpp"
#include "Event.hpp"
#include "MemoryManager.hpp"
#include "OpenGLWrapper.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Shader.hpp"
#include "String.hpp"
#include "ThreadPool.hpp"
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
    struct SpawnData
    {
      SceneObject::Data sceneObject;
      void(*fn)(void*){nullptr};
      void* spawnData{nullptr};
    };

    struct PathData
    {
      GlobalPath path;
      time_t writeTime;
    };

    struct TaskData
    {
      PathData* file{nullptr};
    };

    // Exists only because I'm lazy
    Scene::Data* scene{nullptr};
    AudioSystem::Data audioSystem;
    GlobalDynamicArray<SpawnData> spawnObjects{};
    GlobalDynamicArray<Entity::id> removeObjects{};
    GlobalDynamicArray<GlobalString> audioPaths{};
    float deltaTime{};
    float time{};
#ifdef DEBUG
    GlobalPath dllLockFileEntry = (ApplicationDirectory() / "lock.file").c_str();
    PathData dllLockFile = {dllLockFileEntry.c_str(), dllLockFileEntry.LastWriteTime()};
    ThreadPool::Data threadPool{};
    std::thread hotReloadThread{};
    GlobalHashMap<int, bool, 64> shadersToReload{};
    GlobalDynamicArray<String> levelsToReload{};
    GlobalDynamicArray<PathData> shaders{};
    GlobalDynamicArray<PathData> dlls{};
    GlobalDynamicArray<void*> shaderTaskDatas;
    GlobalDynamicArray<void*> dllTaskDatas;
    std::atomic<bool> stopHotReloadThread{false};
    std::atomic<bool> reload{false};
#endif

#ifdef DEBUG
    void HotReloadThread()
    {
      static auto shaderF = [](void* data) {
          using namespace Render;
          static std::mutex mtx{};
          auto& shader = *static_cast<TaskData*>(data)->file;
          auto shaderTime = shader.path.LastWriteTime();
          if (shaderTime != shader.writeTime)
          {
            shader.writeTime = shaderTime;
            auto globalShaderFiles(GlobalShaderFiles());
            auto it = std::find(globalShaderFiles.begin(),
                                globalShaderFiles.end(),
                                shader.path.buffer.c_str());
            if (it != globalShaderFiles.end())
            {
              std::lock_guard<std::mutex> lock(mtx);
              for (int i = 0; i < ShaderIdx::MAX; ++i)
              {
                shadersToReload.Insert(i);
              }
            }
            else
            {
              size_t pos = std::find(ShaderFiles().begin(),
                                     ShaderFiles().end(),
                                     shader.path.FileName<MemoryManager::Data::THREAD_TEMP>()) -
                           ShaderFiles().begin();
              if (pos < ShaderFiles().size)
              {
                std::lock_guard<std::mutex> lock(mtx);
                shadersToReload.Insert((int)pos);
              }
            }
            reload = true;
          }
        };

      static auto dllF = [](void* data) {
          static std::mutex mtx{};
          auto& dll = *static_cast<TaskData*>(data)->file;
          auto lockFileTime = dllLockFile.path.LastWriteTime();
          if (lockFileTime != dllLockFile.writeTime)
          {
            dllLockFile.writeTime = lockFileTime;
            auto dllTime = dll.path.LastWriteTime();
            if (dllTime != dll.writeTime)
            {
              dll.writeTime = dllTime;
              std::lock_guard<std::mutex> lock(mtx);
              ThreadedString fileName = dll.path.FileName<MemoryManager::Data::THREAD_TEMP>();
              LTrim(RTrim(fileName, ".so"), "lib");
              levelsToReload.PushBack(fileName.c_str());
              reload = true;
            }
          }
        };

      while (!stopHotReloadThread)
      {
        if (!reload)
        {
          ThreadPool::EnqueueForEach(threadPool, shaderTaskDatas.buffer, shaderF, shaderTaskDatas.size);
          ThreadPool::EnqueueForEach(threadPool, dllTaskDatas.buffer, dllF, dllTaskDatas.size);
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
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(stop - start)
                            .count();
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
    scene->sceneFns = &engine.sceneFns.front();
    // Start Render Thread
    Render::Initialize(*scene, windowName, windowX, windowY);

#ifdef DEBUG
    for (const auto& entry : DirectoryContents((AssetsDirectory() / "Shaders").buffer.c_str()))
    {
      if (!entry.Exists())
      {
        continue;
      }
      shaders.PushBack({entry.c_str(), entry.LastWriteTime()});
      shaderTaskDatas.PushBack(MemoryManager::CreateGlobal<TaskData>());
    }

    for (const auto& entry : DirectoryContents(ApplicationDirectory().buffer.c_str()))
    {
      if (!entry.Exists() || !entry.buffer.ends_with(".so"))
      {
        continue;
      }
      dlls.PushBack({entry.c_str(), entry.LastWriteTime()});
      dllTaskDatas.PushBack(MemoryManager::CreateGlobal<TaskData>());
    }

    for (size_t i = 0; i < shaders.size; ++i)
    {
      shaderTaskDatas.PushBack(MemoryManager::CreateGlobal<TaskData>(&shaders[i]));
    }

    for (size_t i = 0; i < dlls.size; ++i)
    {
      dllTaskDatas.PushBack(MemoryManager::CreateGlobal<TaskData>(&dlls[i]));
    }
    ThreadPool::Initialize(threadPool);

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
          scene->sceneFns = nextSceneFns;
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
        nextSceneFns = scene->sceneFns->nextScene;
      }
      break;
      default:
        break;
    }

#ifdef DEBUG
    // Hot reload shaders
    if (reload)
    {
      if (!shadersToReload.Empty())
      {
        Render::OpenGLWrapper::LoadShaders();
        for (auto& shaderIdx : shadersToReload)
        {
          if (shaderIdx.hash != UINT8_MAX)
          {
            Scene::DrawReload(*scene, shaderIdx.key);
          }
        }
        shadersToReload.Clear();
      }
      while (levelsToReload.size > 0)
      {
        auto* copyNextScene = scene->sceneFns;
        while (copyNextScene)
        {
          for (auto& level : levelsToReload)
          {
            if (level.find(copyNextScene->name.c_str()) != SIZE_MAX)
            {
              Scene::LoadSceneFnsFromDynamicLibrary((String(copyNextScene->name.c_str()) + "Level").c_str(), *copyNextScene);
            }
          }
          copyNextScene = copyNextScene->nextScene;
        }
        levelsToReload.PopBack();
      }
      reload = false;
    }
#endif

    // Process events in the renderer
    Render::Run(*scene);
    Input::Process(engine.inputData);

    if (spawnObjects.size > 0)
    {
      Scene::SpawnObject(*scene, spawnObjects.back().sceneObject);
      if (spawnObjects.back().fn)
      {
        spawnObjects.back().fn(spawnObjects.back().spawnData);
      }
      spawnObjects.PopBack();
    }

    if (removeObjects.size > 0)
    {
      Scene::RemoveObject(*scene, removeObjects.back());
      removeObjects.PopBack();
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
      float _deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(stop - start)
                           .count();
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
    Render::OpenGLWrapper::Destruct();
    Scene::Destroy(engine.scene);
    AudioSystem::Destruct(audioSystem);
  }

  void Global::Construct(Engine::Data _engine) { Global::engine = std::move(_engine); }

  void Global::Quit() { engine.quit = true; }

  bool Global::IsActive() { return !engine.quit; }

  bool Global::IsSpawning() { return spawnObjects.size > 0; }

  float Global::DeltaTime() { return deltaTime; }

  float Global::Time() { return time; }

  void Global::SetAudioPaths(const DynamicArray<GlobalString, MemoryManager::Data::GLOBAL_ARENA>& _audioPaths)
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

  void Global::ChangeMasterVolume(float volume) { AudioSystem::ChangeMasterVolume(volume); }

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

  void Global::SpawnObject(SceneObject::Data object, void(*callback)(void*), void* data)
  {
    spawnObjects.PushBack({std::move(object), callback, data});
  }

  void Global::RemoveObject(Entity::id entity) { removeObjects.PushBack(std::move(entity)); }
}
