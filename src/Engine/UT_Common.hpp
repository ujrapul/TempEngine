// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "STDPCH.hpp"
#include "Engine.hpp" // IWYU pragma: keep

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
    ~Timer() { Logger::Log(String(name.c_str()) + ": " + String::ToString(getTime()) + " nanoseconds"); }
  };

  struct MockComponent
  {
    const char* name{""};
    int value{};
    bool toggle{false};

    bool operator==(const MockComponent& other) const = default;
  };

  inline void CleanupScene(Scene::Data& scene)
  {
    auto& objects = scene.objects;
    auto& table = scene.objectsNameIdxTable;
    for (size_t i = 0; i < objects.size; ++i)
    {
      SceneObject::Destruct(scene, objects[i]);
    }
    objects.Clear();
    table.Clear();
  }

  inline void Assert(const char* testName, bool value)
  {
    if (!value)
    {
      printf("FAILED [\033[91m\033[1m %s\033[0m ]\n", testName);
    }
    assert(value);
  }

  inline void Assert(const String& testName, bool value)
  {
    if (!value)
    {
      printf("FAILED [\033[91m\033[1m %s\033[0m ]\n", testName.c_str());
    }
    assert(value);
  }

  template <typename T>
  inline void AssertEqual(const char* testName, T v1, T v2)
  {
    if (v1 != v2)
    {
      inout << "FAILED EQUAL [\033[91m\033[1m " << testName << ":\n" << v1 << v2
            << "\033[0m ]\n";
    }
    assert(v1 == v2);
  }

  template <typename T>
  inline void AssertEqual(const String& testName, T v1, T v2)
  {
    if (v1 != v2)
    {
      inout << "FAILED EQUAL [\033[91m\033[1m " << testName << ":\n" << v1 << v2
              << "\033[0m ]\n";
    }
    assert(v1 == v2);
  }

  template <typename T>
  inline void AssertNotEqual(const char* testName, T v1, T v2)
  {
    if (v1 == v2)
    {
      inout << "FAILED NOT EQUAL [\033[91m\033[1m " << testName << ":\n" << v1 << v2
            << "\033[0m ]\n";
    }
    assert(v1 != v2);
  }

  template <typename T>
  inline void AssertNotEqual(const String& testName, T v1, T v2)
  {
    if (v1 == v2)
    {
      inout << "FAILED NOT EQUAL [\033[91m\033[1m " << testName << ":\n" << v1 << v2
            << "\033[0m ]\n";
    }
    assert(v1 != v2);
  }
}
