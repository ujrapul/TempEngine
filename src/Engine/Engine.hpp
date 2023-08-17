// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Input.hpp"
#include "Math.hpp"
#include "Scene.hpp"
#include <functional>
#include <vector>

struct lua_State;

namespace Temp::Coordinator
{
  struct Data;
}

namespace Temp::Engine
{
  struct RenderData
  {
    void (*func)(void*){nullptr};
    void* data;
  };

  struct Data
  {
    std::vector<Scene::SceneFns> sceneFns{};
    Input::Data inputData{};
    Scene::Data scene{};
    Math::Vec4f backgroundColor{0.2f, 0.2f, 0.2f, 1.0f};
    bool quit{false};

    Data& operator=(const Data& other)
    {
      sceneFns = other.sceneFns;
      inputData = other.inputData;
      scene = other.scene;
      backgroundColor = other.backgroundColor;
      quit = other.quit;
      return *this;
    }
  };
}

// Making these functions global from Engine for ease of use
namespace Temp
{
  // A sort of hacky way of encapsulating global state
  // Perhaps find a better solution later on...
  class Global
  {
  public:
    // ENGINE
    static void Start(const char* windowName, int windowX, int windowY);
    static void Process(float _deltaTime);
    static void Run(const char* windowName, int windowX, int windowY);
    static void Destroy();
    static void Construct(const Engine::Data& _engine);
    static void Quit();
    static bool IsActive();
    static bool IsSpawning();
    static constexpr const Math::Vec4f& GetBackgroundColor() { return engine.backgroundColor; };
    static float DeltaTime();
    static float Time();
    
    // AUDIO
    static void SetAudioPaths(const std::vector<std::string>& _audioPaths);
    static void PlayAudioLoop(int i, std::atomic<bool>& stopHandle);
    static void PlayAudio(int i);
    static void ChangeMasterVolume(float volume);
    static void ChangeVolume(int i, float volume);

    // INPUT
    static void AddPressCallback(void (*callback)(Input::KeyboardCode), Input::KeyboardCode keyCode);
    static void RemovePressCallback(void (*callback)(Input::KeyboardCode), Input::KeyboardCode keyCode);
    static void AddReleaseCallback(void (*callback)(Input::KeyboardCode), Input::KeyboardCode keyCode);
    static void RemoveReleaseCallback(void (*callback)(Input::KeyboardCode), Input::KeyboardCode keyCode);
    static void PushPressKeyQueue(Input::KeyboardCode keyCode);
    static void PushReleaseKeyQueue(Input::KeyboardCode keyCode);

    // SCENE
    // Make sure to use callbacks if you need to modify the object after spawning
    static void SpawnObject(
      const SceneObject::Data& object,
      std::function<void()> callback = []() {});
    static void RemoveObject(const SceneObject::Data& object);

  private:
    static Engine::Data engine;
  };
}
