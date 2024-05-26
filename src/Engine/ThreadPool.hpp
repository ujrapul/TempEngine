// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array_fwd.hpp"
#include "Math.hpp" // IWYU pragma: keep
#include "STDPCH.hpp"

namespace Temp::ThreadPool
{
  struct Task
  {
    void* data{nullptr};
    void(*func)(void*){nullptr};

    constexpr bool operator==(const Task& other) const
    {
      return data == other.data
        && func == other.func;
    }
  };

  // Beware of atomics used in wait
  struct Data
  {
    GlobalDynamicArray<std::thread*> threads{true, 512, nullptr};
    GlobalQueue<Task> tasks{true, 512};
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

  inline void Enqueue(Data& threadPool, void(*f)(void*), void* data)
  {
    {
      std::unique_lock<std::mutex> lock(threadPool.mtx);
      threadPool.tasks.Emplace(Task(data, f));
    }
    threadPool.condition.notify_one();
  }

  inline void EnqueueForEach(Data& threadPool, void**& collection, void (*func)(void*), size_t collectionSize)
  {
    size_t stride = Math::Floor((double)collectionSize / threadPool.threads.size);
    if (stride <= 1)
    {
      for (size_t i = 0; i < collectionSize; ++i)
      {
        Enqueue(threadPool, func, collection[i]);
      }
      return;
    }

    RecreateThreads(threadPool, collectionSize);

    struct TaskData
    {
      size_t begin{0};
      size_t end{0};
      void** collection{nullptr};
      void (*func)(void*){nullptr};
      bool inUse{false};
    };
    static GlobalArray<TaskData, 512> taskDatas;
    for (size_t i = 0; i < threadPool.threads.size; ++i)
    {
      size_t begin = i * stride;
      size_t end = i == threadPool.threads.size - 1 ? collectionSize
                                                    : Math::Min((i + 1) * stride, collectionSize);
      int currTask = -1;
      for (size_t j = 0; j < taskDatas.size; ++j)
      {
        if (!taskDatas[j].inUse)
        {
          taskDatas[i] = {begin, end, collection, func, true};
          currTask = j;
          break;
        }
      }
      if (currTask > -1)
      {
        auto f = [](void* data) -> void {
          auto taskData = static_cast<TaskData*>(data);
          for (size_t i = taskData->begin; i < taskData->end; ++i)
          {
            taskData->func(taskData->collection[i]);
          }
          taskData->inUse = false;
        };
        // std::cout << begin << " " << end << std::endl;
        Enqueue(threadPool, f, &taskDatas[currTask]);

        if (end == collectionSize)
        {
          return;
        }
      }
    }
  }
}
