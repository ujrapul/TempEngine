// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array.hpp"
#include "EngineUtils.hpp"
#include "Logger.hpp"
#include <cstddef>
#include <cstring>

namespace Temp::SaveSerializer
{
  enum class Type
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
    Type type{0};
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
            delete static_cast<bool*>(data.state);
            break;
          case Type::FLOAT:
            delete static_cast<float*>(data.state);
            break;
          case Type::INT:
            delete static_cast<int*>(data.state);
            break;
          case Type::STRING:
            delete static_cast<std::string*>(data.state);
            break;
          case Type::MAX:
            break;
        };
      }
    }
    DynamicArray<Data> datas;
  };

  inline void Save(const DataList& dataList)
  {
    std::ofstream f(ApplicationDirectory() / "Save");

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
    f << "// Save file: " << std::put_time(timeinfo, "%Y-%m-%d") << " "
      << std::put_time(timeinfo, "%H:%M:%S") << "\n\n";
#endif

    for (const auto& data : dataList.datas)
    {
      switch (data.type)
      {
        case Type::BOOL:
          f << *static_cast<bool*>(data.state) << " " << (int)data.type << std::endl;
          break;
        case Type::FLOAT:
          f << *static_cast<float*>(data.state) << " " << (int)data.type << std::endl;
          break;
        case Type::INT:
          f << *static_cast<int*>(data.state) << " " << (int)data.type << std::endl;
          break;
        case Type::STRING:
          f << *static_cast<std::string*>(data.state) << " " << (int)data.type << std::endl;
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
    try
    {
      ReadFile(contents, (ApplicationDirectory() / "Save").c_str());
    }
    catch (const std::exception&)
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
          data.state = new bool((bool)atoi(state.c_str()));
          break;
        case Type::FLOAT:
          data.state = new float((float)atof(state.c_str()));
          break;
        case Type::INT:
          data.state = new int(atoi(state.c_str()));
          break;
        case Type::STRING:
          data.state = new std::string(state.c_str());
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