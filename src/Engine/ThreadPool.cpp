// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "ThreadPool.hpp"
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
  namespace
  {
    void WorkerThread(Data& threadPool, int /*id*/)
    {
      lua_State* L;
      {
        std::lock_guard<std::mutex> lock(threadPool.mtx);
        L = luaL_newstate();
        luaL_openlibs(L);
      }

      while (true)
      {
        std::function<void()> task;
        std::function<void(lua_State*)> luaTask;
        {
          std::unique_lock<std::mutex> lock(threadPool.mtx);
          threadPool.condition.wait(lock, [&threadPool]() {
            return threadPool.stop || !threadPool.tasks.empty() || !threadPool.luaTasks.empty();
          });
          if (threadPool.stop)
          {
            return;
          }
          if (!threadPool.tasks.empty())
          {
            task = std::move(threadPool.tasks.front());
            threadPool.tasks.pop();
          }
          if (!threadPool.luaTasks.empty())
          {
            luaTask = std::move(threadPool.luaTasks.front());
            threadPool.luaTasks.pop();
          }
        }
        threadPool.activeThreads.fetch_add(1);
        if (task)
        {
          task();
        }
        if (luaTask)
        {
          luaTask(L);
        }
        threadPool.activeThreads.fetch_sub(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(0));
      }

      {
        std::lock_guard<std::mutex> lock(threadPool.mtx);
        lua_close(L);
      }
    }

    void AddWorker(Data& threadPool, int id)
    {
      auto f = [&threadPool, id]() { WorkerThread(threadPool, id); };
      threadPool.threads.emplace_back(std::move(f));
    }

    void Initialize(Data& threadPool, int numThreads)
    {
      for (int id = 0; id < numThreads; ++id)
      {
        AddWorker(threadPool, id);
      }
    }

    void Poll(Data& threadPool)
    {
      threadPool.condition.notify_one(); // wake one worker thread
      std::this_thread::yield();         // allow this thread to be rescheduled
    }
  }

  void Initialize(Data& threadPool)
  {
    Initialize(threadPool, Math::Max((int)std::thread::hardware_concurrency(), 2));
  }

  void Destruct(Data& threadPool)
  {
    {
      std::unique_lock<std::mutex> lock(threadPool.mtx);
      threadPool.stop = true;
    }
    threadPool.condition.notify_all();
    for (auto& thread : threadPool.threads)
    {
      thread.join();
    }
    {
      std::unique_lock<std::mutex> lock(threadPool.mtx);
      threadPool.threads.clear();
      threadPool.activeThreads = 0;
      threadPool.tasks = {};
      threadPool.luaTasks = {};
      threadPool.stop = false;
    }
  }

  void RecreateThreads(Data& threadPool, size_t collectionSize)
  {
    // TODO: May need to move this elsewhere
    // to prevent hitching
    constexpr const size_t inMaxRange = 10000;
    size_t inValue = Math::Min(collectionSize, inMaxRange);
    const int outMinRange = std::thread::hardware_concurrency();
    // Scaling thread multiplier based on thread count: 8T - 1, 16T - 2, 32T - 4
    const int outMaxRange = outMinRange *
                            Math::Max(Math::Floor(((float)outMinRange / 32 * (4 - 1) + 1)), 1);
    int upperThread = (int)((float)inValue / inMaxRange * (outMaxRange - outMinRange) +
                            outMinRange);
    if (upperThread != (int)threadPool.threads.size())
    {
      Destruct(threadPool);
      Initialize(threadPool, upperThread);
      // AddWorker(threadPool, threadPool.threads.size());
      threadPool.condition.notify_all();
      // std::cout << "Upper Thread: " << upperThread << std::endl;
    }
  }

  bool IsActive(Data& threadPool)
  {
    std::unique_lock<std::mutex> lock(threadPool.mtx);
    return !threadPool.tasks.empty() || !threadPool.luaTasks.empty() || threadPool.activeThreads.load() > 0;
  }

  void Wait(Data& threadPool)
  {
    while (IsActive(threadPool))
    {
      Poll(threadPool);
    }
  }

  // NOTE: Keeping for reference in case it's needed later
  // template <typename Collection, typename Function>
  // inline void EnqueueForEach(Data& threadPool, Collection& collection, Function&& f)
  // {
  //   .
  //   .
  //   .
  //   if (upperThread < (int)threadPool.threads.size())
  //   {
  //     auto it = threadPool.threads.rbegin();
  //     while (it != threadPool.threads.rend())
  //     {
  //       if (!it->isBusy._a.load())
  //       {
  //         it->isStop._a = true;
  //         it->thread.join();
  //         std::advance(it, 1);
  //         threadPool.threads.erase(it.base());
  //         if (upperThread == (int)threadPool.threads.size())
  //         {
  //           break;
  //         }
  //       }
  //       else
  //       {
  //         ++it;
  //       }
  //     }
  //   }
  // }
}
