// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Engine.hpp"
#include "Logger.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <string>

namespace Temp
{
  struct Timer
  {
    std::string name;
    std::chrono::high_resolution_clock::time_point start;

    Timer(const std::string& name) : name(name), start(std::chrono::high_resolution_clock::now()) {}
    inline int64_t getTime() const
    {
      auto end = std::chrono::high_resolution_clock::now();
      return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }
    ~Timer() { Logger::Log(name + ": " + std::to_string(getTime()) + " nanoseconds"); }
  };

  struct MockComponent
  {
    std::string name{""};
    int value{};
    bool toggle{false};

    bool operator==(const MockComponent& other) const = default;
  };

  inline void CleanupScene(Scene::Data& scene)
  {
    auto& objects = scene.objects;
    auto& table = scene.objectsNameIdxTable;
    for (size_t i = 0; i < objects.size(); ++i)
    {
      SceneObject::Destruct(scene, objects[i]);
    }
    objects.clear();
    table.clear();
  }

  inline void Assert(const char* testName, bool value)
  {
    if (!value)
    {
      std::cout << "FAILED [\033[91m\033[1m " << testName << "\033[0m ]\n";
    }
    assert(value);
  }

  inline void Assert(const std::string& testName, bool value)
  {
    if (!value)
    {
      std::cout << "FAILED [\033[91m\033[1m " << testName << "\033[0m ]\n";
    }
    assert(value);
  }

  template <typename T>
  inline void AssertEqual(const char* testName, T v1, T v2)
  {
    if (v1 != v2)
    {
      std::cout << "FAILED EQUAL [\033[91m\033[1m " << testName << ": " << v1 << " " << v2
                << "\033[0m ]\n";
    }
    assert(v1 == v2);
  }

  template <typename T>
  inline void AssertEqual(const std::string& testName, T v1, T v2)
  {
    if (v1 != v2)
    {
      std::cout << "FAILED EQUAL [\033[91m\033[1m " << testName << ": " << v1 << " " << v2
                << "\033[0m ]\n";
    }
    assert(v1 == v2);
  }

  template <typename T>
  inline void AssertNotEqual(const char* testName, T v1, T v2)
  {
    if (v1 == v2)
    {
      std::cout << "FAILED NOT EQUAL [\033[91m\033[1m " << testName << ": " << v1 << " " << v2
                << "\033[0m ]\n";
    }
    assert(v1 != v2);
  }

  template <typename T>
  inline void AssertNotEqual(const std::string& testName, T v1, T v2)
  {
    if (v1 == v2)
    {
      std::cout << "FAILED NOT EQUAL [\033[91m\033[1m " << testName << ": " << v1 << " " << v2
                << "\033[0m ]\n";
    }
    assert(v1 != v2);
  }
}
