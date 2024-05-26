// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Logger.hpp"
#include "MemoryManager.hpp"
#include "String.hpp"
#include <cstdio>
#include <cstdlib>

namespace Temp
{
  struct FileWriter
  {
    FILE *file;

    FileWriter(const char* path) :
      file(fopen(path, "w"))
    {
      if (!file)
      {
        Logger::LogErr(String("Error opening file: ") + path);
      }
    }

    ~FileWriter()
    {
      if (file)
      {
        fclose(file);
      }
    }

    FileWriter& operator<<(const char* data)
    {
      if (!fputs(data, file))
      {
        Logger::LogErr(String("Failed to write data: ") + data);
      }

      return *this;
    }

    FileWriter& operator<<(int data)
    {
      if (!fprintf(file, "%d", data))
      {
        Logger::LogErr(String("Failed to write data: ") + data);
      }

      return *this;
    }

    FileWriter& operator<<(size_t data)
    {
      if (!fprintf(file, "%zu", data))
      {
        Logger::LogErr(String("Failed to write data: ") + data);
      }

      return *this;
    }

    FileWriter& operator<<(float data)
    {
      if (!fprintf(file, "%f", data))
      {
        Logger::LogErr(String("Failed to write data: ") + data);
      }

      return *this;
    }

    template<MemoryManager::Data::Type type = MemoryManager::Data::TEMP>
    FileWriter& operator<<(const BaseString<type>& data)
    {
      if (!fputs(data.c_str(), file))
      {
        Logger::LogErr(String("Failed to write data: ") + data.c_str());
      }

      return *this;
    }
  };
}