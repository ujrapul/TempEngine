// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "String.hpp"
#include "Logger.hpp"
#include <cstdio>
#ifdef __linux__
#include <sys/stat.h>
#elif __APPLE__
#elif __WIN32
#endif

namespace Temp
{
  template<MemoryManager::Data::Type type = MemoryManager::Data::TEMP>
  struct BasePath
  {
    BaseString<type> buffer;

    constexpr BasePath() {}

    // Copy constructor
    constexpr BasePath(const BasePath& other) noexcept
    {
      buffer = other.buffer;
    }

    // Move constructor
    constexpr BasePath(BasePath&& other) noexcept
      : BasePath()
    {
      Swap(*this, other);
    }

    constexpr BasePath(const char* other) noexcept
      : buffer(other)
    {
    }

    // Does a copy operation
    constexpr BasePath& operator=(BasePath other)
    {
      Swap(*this, other);
      return *this;
    }

    constexpr void Swap(BasePath& first, BasePath& second)
    {
      Utils::Swap(first.buffer, second.buffer);
    }

    constexpr BasePath ParentPath() const
    {
      int idx = -1;
      for (int i = (int)buffer.size; i >= 0; --i)
      {
#ifdef __linux
        if (buffer[i] == '/')
#elif __APPLE__
        if (buffer[i] == '/')
#elif __WIN32
        if (buffer[i] == '\\')
#endif
        {
          idx = i;
          break;
        }
      }
      if (idx == -1)
      {
        return BasePath(buffer.c_str());
      }
      return String(buffer.buffer, buffer.buffer + idx).c_str();
    }

    constexpr BasePath operator/(const BasePath& other) const
    {
      return (buffer + PathSeparator() + other.buffer).c_str();
    }

    constexpr BasePath operator/(const char * other) const
    {
      return (buffer + PathSeparator() + other).c_str();
    }

    constexpr char PathSeparator() const
    {
#ifdef __linux
      return '/';
#elif __APPLE__
      return '/';
#elif __WIN32
      return '\\';
#endif
    }

    constexpr const char* c_str() const
    {
      return buffer.c_str();
    }

    constexpr time_t LastWriteTime() const
    {
#ifdef __linux
      struct stat fileStat;
      if (stat(buffer.c_str(), &fileStat) != 0)
      {
        Logger::LogErr("Could not retrieve write time of file!");
        return -1;
      }
      return fileStat.st_mtime;
#elif __APPLE__
#elif __WIN32
#endif      
    }

    template<MemoryManager::Data::Type fileNameType = MemoryManager::Data::TEMP>
    inline BaseString<fileNameType> FileName() const
    {
      int idx = -1;
      for (int i = (int)buffer.size; i >= 0; --i)
      {
#ifdef __linux
        if (buffer[i] == '/')
#elif __APPLE__
        if (buffer[i] == '/')
#elif __WIN32
        if (buffer[i] == '\\')
#endif
        {
          idx = i;
          break;
        }
      }
      if (idx == -1)
      {
        return buffer.c_str();
      }
      return BaseString<fileNameType>(buffer.buffer + idx + 1, buffer.buffer + buffer.size);
    }

    constexpr bool Exists() const
    {
      FILE* fp = fopen(buffer.c_str(), "rb");
      if (!fp)
      {
        return false;
      }
      fclose(fp);
      return true;
    }

    static bool Remove(const BasePath& path)
    {
      return remove(path.c_str());
    }
  };

  typedef BasePath<> Path;
  typedef BasePath<MemoryManager::Data::SCENE_ARENA> ScenePath;
  typedef BasePath<MemoryManager::Data::GLOBAL_ARENA> GlobalPath;
  typedef BasePath<MemoryManager::Data::THREAD_TEMP> ThreadPath;
}