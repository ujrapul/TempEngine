// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Math.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <sstream>
#include <string.h>
#include <string>
#include <vector>
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

  inline void ReadFile(std::string& contents, const std::string& path)
  {
    std::ifstream in(path, std::ios::in | std::ios::binary);
    if (in)
    {
      in.seekg(0, std::ios::end);
      contents.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(&contents[0], contents.size());
      in.close();
    }
    else
    {
      throw std::runtime_error("Failed to read file: " + path);
    }
  }

  inline std::vector<std::string_view> SplitString(const std::string_view str,
                                                   const char* delimiter,
                                                   int split = 0)
  {
    std::vector<std::string_view> tokens;
    std::size_t start = 0;
    std::size_t end = str.find(delimiter);

    int currSplit = 0;
    while (end != std::string_view::npos)
    {
      tokens.push_back(str.substr(start, end - start));
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
      tokens.push_back(str.substr(start));
    }

    return tokens;
  }

  constexpr std::string_view LTrim(std::string_view str,
                                   std::string const& whitespace = " \r\n\t\v\f")
  {
    const auto pos(str.find_first_not_of(whitespace));
    str.remove_prefix(Math::Min(pos, str.length()));
    return str;
  }

  constexpr std::string_view RTrim(std::string_view str,
                                   std::string const& whitespace = " \r\n\t\v\f")
  {
    const auto pos(str.find_last_not_of(whitespace));
    str.remove_suffix(Math::Min(str.length() - pos - 1, str.length()));
    return str;
  }

  constexpr std::string_view Trim(std::string_view str,
                                  std::string const& whitespace = " \r\n\t\v\f")
  {
    return LTrim(RTrim(str, whitespace), whitespace);
  }
}
