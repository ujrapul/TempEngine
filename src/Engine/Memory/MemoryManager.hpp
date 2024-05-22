// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "LinearAllocator.hpp"
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <mutex>
#include <utility>

namespace Temp::MemoryManager
{
  struct Data
  {
    enum Type
    {
      SCENE_ARENA,
      GLOBAL_ARENA,
      TEMP,
      THREAD_TEMP,
      MAX
    };

    LinearAllocator::Data globalArena{};
    LinearAllocator::Data sceneArena{};
    LinearAllocator::Data tempArena{};
    LinearAllocator::Data threadTempArena{};
    LinearAllocator::TempData tempData{};
    LinearAllocator::TempData threadTempData{};
    size_t tempDataSize{0};
    size_t threadTempDataSize{0};
    // size_t sceneDataSize{0};
    std::mutex mtx;

    Data()
    {
      // Allocate 134 mb of memory
      const size_t length = 134217728;
      void* sceneBuffer = malloc(length);
      LinearAllocator::Init(sceneArena, sceneBuffer, length);

      void* globalBuffer = malloc(length);
      LinearAllocator::Init(globalArena, globalBuffer, length);

      // Allocate 1073 mb of memory
      const size_t tempLength = 1073741824;
      void* tempBuffer = malloc(tempLength);
      LinearAllocator::Init(tempArena, tempBuffer, tempLength);

      // Allocate 1073 mb of memory
      void* threadTempBuffer = malloc(tempLength);
      LinearAllocator::Init(threadTempArena, threadTempBuffer, tempLength);
    }

    ~Data()
    {
      free(sceneArena.buffer);
      free(globalArena.buffer);
      free(tempArena.buffer);
      free(threadTempArena.buffer);
    }

    void* Allocate(Type type, size_t size)
    {
      switch (type)
      {
        case SCENE_ARENA:
        {
          // ++sceneDataSize;
          // Logger::LogErr(std::to_string(sceneDataSize));
          // Logger::LogErr("Scene Memory Usage in MB: " + std::to_string(sceneArena.offset / 1000000.f));
          return LinearAllocator::Alloc(sceneArena, size);
        }
        case GLOBAL_ARENA:
          // Logger::LogErr("Global Memory Usage in MB: " + std::to_string(globalArena.offset / 1000000.f));
          return LinearAllocator::Alloc(globalArena, size);
        case TEMP:
          // Logger::LogErr("Begin Temp Memory Usage in MB: " + std::to_string(tempArena.offset / 1000000.f));
          if (tempDataSize == 0)
          {
            tempData = LinearAllocator::TempMemoryBegin(tempArena);
          }
          ++tempDataSize;
          return LinearAllocator::Alloc(tempArena, size);
        case THREAD_TEMP:
          if (threadTempDataSize == 0)
          {
            threadTempData = LinearAllocator::TempMemoryBegin(threadTempArena);
          }
          ++threadTempDataSize;
          // Logger::LogErr("BEG: " + std::to_string(threadTempDataSize));
          // Logger::LogErr("BEG THREAD TMP Memory Usage in MB: " + std::to_string(threadTempArena.offset / 1000000.f));
          return LinearAllocator::Alloc(threadTempArena, size);
        case MAX:
        default:
          return nullptr;
      }
      return nullptr;
    }

    size_t GetCurrentOffset(Type type)
    {
      switch (type)
      {
        case SCENE_ARENA:
          return sceneArena.offset;
        case GLOBAL_ARENA:
          return globalArena.offset;
        case TEMP:
          return tempArena.offset;
        case THREAD_TEMP:
          return threadTempArena.offset;
        case MAX:
        default:
          return 0;
      }
      return 0;
    }

    void Free(Type type, size_t /*prevOffset*/)
    {
      switch (type)
      {
        case SCENE_ARENA:
          // Do nothing
          break;
        case GLOBAL_ARENA:
          // Do nothing
          break;
        case TEMP:
          --tempDataSize;
          assert(tempDataSize != SIZE_MAX);
          if (tempDataSize == 0)
          {
            LinearAllocator::TempMemoryEnd(tempData);
          }
          // Logger::LogErr("End Temp Memory Usage in MB:   " + std::to_string(tempArena.offset / 1000000.f));
          break;
        case THREAD_TEMP:
          --threadTempDataSize;
          assert(threadTempDataSize != SIZE_MAX);
          if (threadTempDataSize == 0)
          {
            LinearAllocator::TempMemoryEnd(threadTempData);
          }
          // std::lock_guard<std::mutex> lock(mtx);
          // Logger::LogErr("END: " + std::to_string(threadTempDataSize));
          // Logger::LogErr("END THREAD TMP Memory Usage in MB: " + std::to_string(threadTempArena.offset / 1000000.f));
          break;
        case MAX:
        default:
          break;
      }
    }

    void FreeAll()
    {
      // sceneDataSize = 0;
      // Logger::LogErr("//////////////////////////////////////////////////////////////////////////");
      LinearAllocator::FreeAll(sceneArena);
      LinearAllocator::FreeAll(tempArena);
      LinearAllocator::FreeAll(threadTempArena);
    }
  };

  inline MemoryManager::Data data{};

  template <typename T>
  T* Create(MemoryManager::Data::Type type = Data::TEMP)
  {
    return static_cast<T*>(MemoryManager::data.Allocate(type, sizeof(T)));
  }

  template <typename T, typename... A>
  T* Create(MemoryManager::Data::Type type, A... args)
  {
    T* ptr = static_cast<T*>(MemoryManager::data.Allocate(type, sizeof(T)));
    new (ptr) T(args...);
    return ptr;
  }

  template <typename T, typename... A>
  T* CreateTemp(A... args)
  {
    T* ptr = static_cast<T*>(MemoryManager::data.Allocate(MemoryManager::Data::Type::TEMP, sizeof(T)));
    new (ptr) T(std::forward<A>(args)...);
    return ptr;
  }

  template <typename T, typename... A>
  T* CreateScene(A... args)
  {
    T* ptr = static_cast<T*>(MemoryManager::data.Allocate(MemoryManager::Data::Type::SCENE_ARENA, sizeof(T)));
    new (ptr) T(std::forward<A>(args)...);
    return ptr;
  }

  template <typename T, typename... A>
  T* CreateGlobal(A... args)
  {
    T* ptr = static_cast<T*>(MemoryManager::data.Allocate(MemoryManager::Data::Type::GLOBAL_ARENA, sizeof(T)));
    new (ptr) T(std::forward<A>(args)...);
    return ptr;
  }

  // Only meant for single-threaded usage
  struct ScopedTempMemory
  {
    LinearAllocator::TempData temp;
    size_t tempDataSize{0};

    ScopedTempMemory()
    {
      temp = LinearAllocator::TempMemoryBegin(data.tempArena);
      tempDataSize = MemoryManager::data.tempDataSize;
    }

    ~ScopedTempMemory()
    {
      LinearAllocator::TempMemoryEnd(temp);
      MemoryManager::data.tempDataSize = tempDataSize;
    }
  };
};