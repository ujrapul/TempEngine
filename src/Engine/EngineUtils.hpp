// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array_fwd.hpp"
#include "Logger.hpp" // IWYU pragma: keep
#include "Math_fwd.hpp"
#include "MemoryManager.hpp"
#include "String.hpp"
#include "FileSystem.hpp"
#include <cstdint>
#include <cstdio>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <dirent.h>

#include <pthread.h>
#include <sys/syscall.h>
#include <linux/limits.h>
#endif

namespace Temp
{
  Path ApplicationDirectory();
  Path AssetsDirectory();
  const GlobalPath& ApplicationDirectoryGlobal();
  const GlobalPath& AssetsDirectoryGlobal();

  template <typename T>
  inline void FreeContainer(T& p_container)
  {
    T empty;
    p_container.swap(empty);
  }

  template<MemoryManager::Data::Type type = MemoryManager::Data::TEMP>
  inline bool ReadFile(BaseString<type>& contents, const char* path, String& output)
  {
    FILE *fp = fopen(path, "rb");
    if (!fp)
    {
      output = (String("Failed to read file: ") + path).c_str();
      return false;
    }
    long length;
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    contents.Resize(length + 1);
    fread(contents.buffer, 1, length, fp);
    contents.size = length + 1;
    fclose(fp);
    return true;
  }

  template<MemoryManager::Data::Type type = MemoryManager::Data::TEMP>
  inline BaseString<type> LoadFileAsString(const char* filePath, bool newLine = false)
  {
    BaseString<type> contents;
    FILE* fp = fopen(filePath, "rb");
    if (!fp)
    {
      Logger::LogErr(String("Failed to open file: ") + filePath);
      return contents;
    }
    long length;
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    contents.Resize(length + 2);
    String output;
    if (!ReadFile(contents, filePath, output))
    {
      Logger::LogErr(output.c_str());
    }
    if (newLine)
    {
      contents += "\n";
    }

    return contents;
  }

  inline DynamicArray<String> SplitString(const String& str, const char* delimiter, int split = 0)
  {
    // Make sure enough memory is reserved up front so that there isn't a reallocation
    DynamicArray<String> tokens(true, Math::Max(str.count(delimiter) * 2, 8ul));
    std::size_t start = 0;
    std::size_t end = str.find(delimiter);

    int currSplit = 0;
    while (end != SIZE_MAX)
    {
      tokens.PushBack(str.substr(start, end));
      start = end + strlen(delimiter);
      end = str.find(delimiter, start);
      ++currSplit;
      if (split > 0 && currSplit >= split)
      {
        break;
      }
    }

    if (!str.substr(start).empty())
    {
      tokens.PushBack(str.substr(start));
    }

    return tokens;
  }

  template<MemoryManager::Data::Type type = MemoryManager::Data::TEMP>
  inline BaseString<type>& LTrim(BaseString<type>& str, const char* whitespace = " \r\n\t\v\f")
  {
    const auto pos(str.find_first_not_of(whitespace));
    str.remove_prefix(Math::Min(pos, str.size));
    return str;
  }

  template<MemoryManager::Data::Type type = MemoryManager::Data::TEMP>
  inline BaseString<type>& RTrim(BaseString<type>& str, const char* whitespace = " \r\n\t\v\f")
  {
    const auto pos(str.find_last_not_of(whitespace));
    str.remove_suffix(Math::Min(str.size - pos - 1, str.size));
    return str;
  }

  template<MemoryManager::Data::Type type = MemoryManager::Data::TEMP>
  inline BaseString<type>& Trim(BaseString<type>& str, const char* whitespace = " \r\n\t\v\f")
  {
    return LTrim(RTrim(str, whitespace), whitespace);
  }

  inline bool Contains(const SceneDynamicArray<SceneString>& container, const char* match)
  {
    for (size_t i = 0; i < container.size; ++i)
    {
      if (String(container[i].c_str()) == String(match))
      {
        return true;
      }
    }
    return false;
  }

  inline bool Contains(const GlobalDynamicArray<GlobalString>& container, const char* match)
  {
    for (size_t i = 0; i < container.size; ++i)
    {
      if (String(container[i].c_str()) == String(match))
      {
        return true;
      }
    }
    return false;
  }

  void* OpenDynamicLibrary(const char* name);
  void* GetDynamicLibraryFn(void* libraryHandle, const char* fn);
  void CloseDynamicLibrary(void* libraryHandle);

  // Recursively get all files under directory
  inline DynamicArray<Path> DirectoryContents(const char* path)
  {
    DynamicArray<Path> out;
#if defined(__linux__) || defined(__APPLE__)
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if (d) {
      while ((dir = readdir(d)) != NULL)
      {
        switch (dir->d_type)
        {
          case DT_REG:
            out.PushBack(Path(path) / dir->d_name);
            break;
          case DT_DIR:
          {
            if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") != 0)
            {
              auto tempPaths = DirectoryContents(dir->d_name);
              out.InsertEnd(tempPaths.buffer, tempPaths.size);
            }
            break;
          }
          default:
            break;
        }
      }
      closedir(d);
    }
    return out;
#elif defined(_WIN32)
#endif
  }

  inline size_t GetStackUsage()
  {
#if defined(__linux__)
    pthread_t thread = pthread_self();
    pthread_attr_t attr;
    pthread_getattr_np(thread, &attr);

    void* stack_addr;
    size_t stack_size;
    pthread_attr_getstack(&attr, &stack_addr, &stack_size);

    pthread_attr_destroy(&attr);

    // Calculate the stack usage by comparing the current stack pointer with the start of the stack.
    // This is just an approximation.
    void* current_stack_ptr;
    asm("mov %%rsp, %0" : "=r"(current_stack_ptr));

    return static_cast<char*>(stack_addr) + stack_size - static_cast<char*>(current_stack_ptr);
#elif defined(__APPLE__)
#elif defined(_WIN32)
#endif
  }
}
