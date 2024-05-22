// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Input.hpp"
#include "Math.hpp"
#include "Scene.hpp"

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
    GlobalDynamicArray<Scene::SceneFns> sceneFns{};
    Input::Data inputData{};
    Scene::Data scene{};
    Math::Vec4f backgroundColor{0.2f, 0.2f, 0.2f, 1.0f};
    bool quit{false};

    Data(){}

    Data(Data& other)
      : sceneFns(other.sceneFns),
        inputData(other.inputData),
        scene(other.scene),
        backgroundColor(other.backgroundColor),
        quit(other.quit)
    {
      // ignored
    }

    Data(Data&& other)
      : Data()
    {
      Swap(*this, other);
    }

    Data& operator=(Data other)
    {
      Swap(*this, other);
      return *this;
    }

    constexpr void Swap(Data& first, Data& second)
    {
      Utils::Swap(first.sceneFns, second.sceneFns);
      Utils::Swap(first.inputData, second.inputData);
      Utils::Swap(first.scene, second.scene);
      Utils::Swap(first.backgroundColor, second.backgroundColor);
      Utils::Swap(first.quit, second.quit);
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
    static void Construct(Engine::Data _engine);
    static void Quit();
    static bool IsActive();
    static bool IsSpawning();
    static constexpr const Math::Vec4f& GetBackgroundColor() { return engine.backgroundColor; };
    static float DeltaTime();
    static float Time();
    
    // AUDIO
    static void SetAudioPaths(const DynamicArray<GlobalString, MemoryManager::Data::GLOBAL_ARENA>& _audioPaths);
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
      SceneObject::Data object,
      void(*callback)(void*) = nullptr,
      void* data = nullptr);
    static void RemoveObject(Entity::id entity);

  private:
    static Engine::Data engine;
  };
}
