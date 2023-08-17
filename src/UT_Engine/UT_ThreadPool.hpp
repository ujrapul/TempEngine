// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "ComponentType.hpp"
#include "EntityType.hpp"
#include "Logger.hpp"
#include "Luable.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "ThreadPool.hpp"
#include "UT_Common.hpp"
#include "lua.h"
#include <array>

namespace Temp::ThreadPool::UnitTests
{
  inline void LuaExec(lua_State*, float)
  {
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
  }

  inline void Run()
  {
    SceneObject::Init();
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    Scene::Data scene;
    Scene::Initialize(scene);

    AssertEqual("Test ThreadPool Number of Threads",
                (unsigned int)scene.threadPool.threads.size(),
                std::thread::hardware_concurrency());

    std::array<Mock::Data, 500> mock;
    for (int i = 0; i < 500; ++i)
    {
      Scene::AddComponent<Component::Type::LUABLE>(scene,
                                                   i,
                                                   {
                                                     .path = "Test.lua",
                                                     .luaExec = LuaExec,
                                                   });
      Scene::AddComponent<Component::Type::UPDATEABLE>(scene, i, {&mock, EntityType::MOCK});
    }

    std::array<int, 10000> dummy;
    Component::ArrayData<Component::Luable::Data> dummyLua;
    ThreadPool::EnqueueForEach(scene.threadPool, dummy, [](int) {
      std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    });
    ThreadPool::EnqueueLuaForEach(scene.threadPool,
                                  dummyLua,
                                  [](Component::Luable::Data&, lua_State*) {
                                    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
                                  });
    Assert("Test ThreadPool Number of Active Threads",
           (unsigned int)scene.threadPool.activeThreads.load() > 0);

    ThreadPool::Wait(scene.threadPool);

    Assert("Test ThreadPool Finished",
           scene.threadPool.tasks.size() == 0 && scene.threadPool.luaTasks.size() == 0);

    int64_t time1, time2;
    {
      auto timer = Timer("Single Update:");
      for (int i = 0; i < 10; ++i)
        Scene::UpdateLegacy(scene, L, 0);
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
    lua_close(L);
  }
}