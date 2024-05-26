// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "STDPCH.hpp"

namespace Temp::SaveSerializer
{
  enum Type
  {
    BOOL = 0,
    FLOAT,
    INT,
    STRING,
    MAX,
  };

  struct Data
  {
    void* state{nullptr};
    Type type{Type::MAX};
  };

  struct DataList
  {
    ~DataList()
    {
      for (auto& data : datas)
      {
        switch (data.type)
        {
          case Type::BOOL:
            break;
          case Type::FLOAT:
            break;
          case Type::INT:
            break;
          case Type::STRING:
            static_cast<String*>(data.state)->~String();
            break;
          case Type::MAX:
            break;
        };
        MemoryManager::data.Free(MemoryManager::Data::TEMP, 0);
      }
    }
    DynamicArray<Data> datas;
  };

  inline void Save(const DataList& dataList)
  {
    FileWriter f((ApplicationDirectory() / "Save").c_str());

    // Get the current time point using the system clock
    auto currentTime = std::chrono::system_clock::now();

    // Convert the time point to a time_t representation
    std::time_t time = std::chrono::system_clock::to_time_t(currentTime);
#ifdef _WIN32
    std::tm timeinfo;
    localtime_s(&timeinfo, &time);
    f << "// Save file: " << std::put_time(&timeinfo, "%Y-%m-%d") << " "
      << std::put_time(&timeinfo, "%H:%M:%S") << "\n\n";
#else
    std::tm* timeinfo = std::localtime(&time);
    const char * format = "%d-%d-%d";
    const char * format2 = "%d:%d:%d";
    String timeBuffer;
    timeBuffer.Resize(strlen(format) * 2);
    snprintf(timeBuffer.buffer, strlen(format), format, timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday);
    String timeBuffer2;
    timeBuffer2.Resize(strlen(format2) * 2);
    snprintf(timeBuffer2.buffer, strlen(format2), format2, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    f << "// Save file: " << timeBuffer << " " << timeBuffer2 << "\n\n";
#endif

    for (const auto& data : dataList.datas)
    {
      switch (data.type)
      {
        case Type::BOOL:
          f << *static_cast<bool*>(data.state) << " " << (int)data.type << "\n";
          break;
        case Type::FLOAT:
          f << *static_cast<float*>(data.state) << " " << (int)data.type << "\n";
          break;
        case Type::INT:
          f << *static_cast<int*>(data.state) << " " << (int)data.type << "\n";
          break;
        case Type::STRING:
          f << *static_cast<String*>(data.state) << " " << (int)data.type << "\n";
          break;
        case Type::MAX:
        default:
          break;
      }
    }
  }

  inline bool Load(DataList& dataList)
  {
    String contents;
    String output;
    if (!ReadFile(contents, (ApplicationDirectory() / "Save").c_str(), output))
    {
      Logger::LogErr("[[SaveSerializer]] Failed to deserialize file: Save");
      return false;
    }

    char* line = contents.Buffer();
    while (line)
    {
      char* nextline = strchr(line, '\n');
      size_t length = nextline ? nextline - line : strlen(line);
      String tempLine(line, line + length);
      Trim(tempLine).c_str();
      line = nextline ? nextline + 1 : nullptr;
      if (tempLine.starts_with("//") || tempLine.empty())
      {
        continue;
      }
      auto split = SplitString(tempLine.c_str(), " ");
      if (split.size < 2)
      {
        return false;
      }
      auto state = split[0];
      Data data;
      data.type = static_cast<Type>(atoi(split[1].c_str()));
      switch (data.type)
      {
        case Type::BOOL:
          data.state = MemoryManager::CreateTemp<bool>((bool)atoi(state.c_str()));
          break;
        case Type::FLOAT:
          data.state = MemoryManager::CreateTemp<float>((float)atof(state.c_str()));
          break;
        case Type::INT:
          data.state = MemoryManager::CreateTemp<int>(atoi(state.c_str()));
          break;
        case Type::STRING:
          data.state = MemoryManager::CreateTemp<String>(state.c_str());
          break;
        case Type::MAX:
        default:
          break;
      }
      dataList.datas.PushBack(data);
    }
    return true;
  }
}