// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "ComponentData.hpp"
#include "Entity.hpp"
#include "Luable.hpp"
#include "Math.hpp"
#include "lua.h"
#include "lua.hpp"
#include <algorithm>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

namespace Temp::ThreadPool
{
  // Beware of atomics used in wait
  struct Data
  {
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::queue<std::function<void(lua_State*)>> luaTasks;
    std::mutex mtx;
    std::condition_variable condition;
    std::atomic<int> activeThreads;
    bool stop{false};
  };

  void Initialize(Data& threadPool);
  void Destruct(Data& threadPool);
  void RecreateThreads(Data& threadPool, size_t collectionSize);
  bool IsActive(Data& threadPool);
  void Wait(Data& threadPool);

  template <typename Function, typename... Args>
  inline void Enqueue(Data& threadPool, Function&& f, Args&&... args)
  {
    {
      std::unique_lock<std::mutex> lock(threadPool.mtx);
      threadPool.tasks.emplace(
        [f = std::forward<Function>(f), args = std::make_tuple(std::forward<Args>(args)...)]() {
          std::apply(f, args);
        });
    }
    threadPool.condition.notify_one();
  }

  template <typename Function, typename... Args>
  inline void EnqueueLuaTask(Data& threadPool, Function&& f, Args&&... args)
  {
    {
      std::unique_lock<std::mutex> lock(threadPool.mtx);
      threadPool.luaTasks.emplace(
        [f = std::forward<Function>(f), args = std::make_tuple(std::forward<Args>(args)...)](
          lua_State* L) { std::apply(f, std::tuple_cat(std::make_tuple(L), args)); });
    }
    threadPool.condition.notify_one();
  }

  template <typename Collection, typename Function, typename EnqueueFunc, typename... Args>
  inline void EnqueueForEachCommon(
    Data& threadPool,
    Collection& collection,
    Function f,
    EnqueueFunc&& enqueueFunc, // New template parameter for Enqueue function
    size_t collectionSize)
  {
    size_t stride = Math::Floor((double)collectionSize / threadPool.threads.size());
    if (stride <= 1)
    {
      for (size_t i = 0; i < collectionSize; ++i)
      {
        auto func = [&collection, f, i](Args&&... args) -> void { f(collection[i], args...); };
        enqueueFunc(threadPool, std::move(func));
      }
      return;
    }

    RecreateThreads(threadPool, collectionSize);

    for (size_t i = 0; i < threadPool.threads.size(); ++i)
    {
      size_t begin = i * stride;
      size_t end = i == threadPool.threads.size() - 1
                  ? collectionSize
                  : Math::Min((i + 1) * stride, collectionSize);
      auto func = [&collection, f, begin, end](Args&&... args) -> void {
        for (size_t i = begin; i < end; ++i)
        {
          f(collection[i], args...);
        }
      };
      // std::cout << begin << " " << end << std::endl;
      enqueueFunc(threadPool, std::move(func));

      if (end == collectionSize)
      {
        return;
      }
    }
  }

  template <typename Collection, typename Function>
  inline void EnqueueForEach(Data& threadPool, Collection& collection, Function&& f)
  {
    EnqueueForEachCommon(
      threadPool,
      collection,
      f,
      [](Data& threadPool, std::function<void()>&& func) { Enqueue(threadPool, std::move(func)); },
      collection.size());
  }

  template <typename Collection, typename Function>
  inline void EnqueueForEach(Data& threadPool, Collection& collection, Function&& f, size_t collectionSize)
  {
    EnqueueForEachCommon(
      threadPool,
      collection,
      f,
      [](Data& threadPool, std::function<void()>&& func) { Enqueue(threadPool, std::move(func)); },
      collectionSize);
  }

  inline void EnqueueLuaForEach(Data& threadPool,
                                Component::ArrayData<Component::Luable::Data>& collection,
                                std::function<void(Component::Luable::Data& luable, lua_State*)> f)
  {
    EnqueueForEachCommon<std::array<Component::Luable::Data, Entity::MAX>,
                         std::function<void(Component::Luable::Data & luable, lua_State*)>,
                         std::function<void(Data & threadPool, std::function<void(lua_State*)> &&)>,
                         lua_State*>(
      threadPool,
      collection.array,
      std::move(f),
      [](Data& threadPool, std::function<void(lua_State*)>&& func) {
        EnqueueLuaTask(threadPool, std::move(func));
      },
      collection.size);
  }
}
