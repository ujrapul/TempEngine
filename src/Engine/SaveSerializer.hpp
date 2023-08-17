// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "EngineUtils.hpp"
#include "Logger.hpp"
#include <chrono>
#include <format>
#include <fstream>
#include <string>
#include <vector>

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
    std::vector<Data> datas;
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
    std::string contents;
    try
    {
      ReadFile(contents, (ApplicationDirectory() / "Save").string());
    }
    catch (const std::exception&)
    {
      Logger::LogErr("[[SaveSerializer]] Failed to deserialize file: Save");
      return false;
    }

    std::istringstream f(contents);
    std::string line;
    line.reserve(4096);

    while (std::getline(f, line))
    {
      line = Trim(line);
      if (line.starts_with("//") || line.empty())
      {
        continue;
      }
      auto split = SplitString(line, " ");
      if (split.size() < 2)
      {
        return false;
      }
      auto state = split[0];
      Data data;
      data.type = static_cast<Type>(atoi(split[1].data()));
      switch (data.type)
      {
        case Type::BOOL:
          data.state = new bool((bool)atoi(state.data()));
          break;
        case Type::FLOAT:
          data.state = new float((float)atof(state.data()));
          break;
        case Type::INT:
          data.state = new int(atoi(state.data()));
          break;
        case Type::STRING:
          data.state = new std::string(state);
          break;
        case Type::MAX:
        default:
          break;
      }
      dataList.datas.push_back(data);
    }
    return true;
  }
}