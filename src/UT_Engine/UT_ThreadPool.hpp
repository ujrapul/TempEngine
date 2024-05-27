// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "ComponentType.hpp"
#include "EntityType.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "ThreadPool.hpp"
#include "UT_Common.hpp"
#include <array>

namespace Temp::ThreadPool::UnitTests
{
  inline void Run()
  {
    SceneObject::Init();
    ThreadPool::Data threadPool{};
    ThreadPool::Initialize(threadPool);

    Scene::SceneFns sceneFns;
    Scene::Data scene;
    scene.sceneFns = &sceneFns;
    Scene::Initialize(scene);

    AssertEqual("Test ThreadPool Number of Threads",
                (unsigned int)threadPool.threads.size,
                std::thread::hardware_concurrency());

    std::array<Mock::Data, 500> mock;
    for (int i = 0; i < 500; ++i)
    {
      Scene::AddComponent<Component::Type::UPDATEABLE>(scene, i, {&mock, EntityType::MOCK});
    }

    void** dummyTasks = (void**)malloc(sizeof(void*) * 10000);
    ThreadPool::EnqueueForEach(threadPool, dummyTasks, [](void*) {
      std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    }, 10000);
    Assert("Test ThreadPool Number of Active Threads",
           (unsigned int)threadPool.activeThreads.load() > 0);

    ThreadPool::Wait(threadPool);

    int64_t time1, time2;
    {
      auto timer = Timer("Single Update:");
      for (int i = 0; i < 10; ++i)
        Scene::UpdateLegacy(scene, 0);
      time1 = timer.getTime();
    }

    {
      auto timer = Timer("Multi Update: ");
      for (int i = 0; i < 10; ++i)
        Scene::Update(scene, 0);
      time2 = timer.getTime();
    }
    Assert("ThreadPool is Faster", time2 < time1);

    Scene::Destruct(scene);
    Scene::Destroy(scene);
    ThreadPool::Destruct(threadPool);
  }
}