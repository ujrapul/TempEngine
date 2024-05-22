// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array_fwd.hpp"
#include "Logger.hpp"
#include "Math_fwd.hpp"
#include "STDPCH.hpp"
#include "String.hpp"
#include <cstdint>
#include <cstdio>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace Temp
{
  const std::filesystem::path& ApplicationDirectory();
  const std::filesystem::path& AssetsDirectory();

  inline const char* LoadFileAsString(const char* filePath, bool nullTerminate = true)
  {
    std::ifstream fileStream(filePath, std::ios::ate | std::ios::binary);
    if (!fileStream)
    {
      std::cerr << "Failed to open file: " << filePath << std::endl;
      return nullptr;
    }

    std::streamsize fileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);

    // Allocate memory for the file content
    char* charArray = new char[fileSize + 1];
    fileStream.read(charArray, fileSize);
    if (nullTerminate)
    {
      charArray[fileSize] = '\0';
    }
    else
    {
      charArray[fileSize] = '\n';
    }

    return charArray;
  }

  template <typename T>
  inline void FreeContainer(T& p_container)
  {
    T empty;
    p_container.swap(empty);
  }

  inline void ReadFile(String& contents, const char* path)
  {
    FILE *fp = fopen(path, "rb");
    if (!fp)
    {
      throw std::runtime_error((String("Failed to read file: ") + path).c_str());
    }
    long length;
    char* content;
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    content = (char *)malloc(length + 1);
    fread(content, 1, length, fp);
    content[length] = '\0';
    contents = content;
    free(content);
  }

  inline DynamicArray<String> SplitString(const String& str, const char* delimiter, int split = 0)
  {
    // Make sure enough memory is reserved up front so that there isn't a reallocation
    DynamicArray<String> tokens{true, Math::Max(str.count(delimiter) * 2, 8ul)};
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

  inline String& LTrim(String& str, const char* whitespace = " \r\n\t\v\f")
  {
    const auto pos(str.find_first_not_of(whitespace));
    str.remove_prefix(Math::Min(pos, str.size));
    return str;
  }

  inline String& RTrim(String& str, const char* whitespace = " \r\n\t\v\f")
  {
    const auto pos(str.find_last_not_of(whitespace));
    str.remove_suffix(Math::Min(str.size - pos - 1, str.size));
    return str;
  }

  inline String& Trim(String& str, const char* whitespace = " \r\n\t\v\f")
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

  void* OpenDynamicLibrary(const std::string& name);
  void* GetDynamicLibraryFn(void* libraryHandle, const std::string& fn);
  void CloseDynamicLibrary(void* libraryHandle);
}
