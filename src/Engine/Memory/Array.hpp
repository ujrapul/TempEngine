// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array_impl.hpp"
#include "MemoryManager.hpp"

namespace Temp
{
  extern template struct DynamicArray<float, MemoryManager::Data::TEMP>;
  extern template struct DynamicArray<float, MemoryManager::Data::GLOBAL_ARENA>;
  extern template struct DynamicArray<float, MemoryManager::Data::SCENE_ARENA>;
  extern template struct DynamicArray<float, MemoryManager::Data::THREAD_TEMP>;
  extern template struct DynamicArray<char const*, (Temp::MemoryManager::Data::Type)1>;
  extern template struct DynamicArray<Temp::DynamicArray<char const*, (Temp::MemoryManager::Data::Type)1>, (Temp::MemoryManager::Data::Type)1>;
  extern template struct Temp::DynamicArray<unsigned int, (Temp::MemoryManager::Data::Type)0>;
  extern template struct Temp::DynamicArray<unsigned long, (Temp::MemoryManager::Data::Type)0>;
  extern template struct Temp::DynamicArray<bool, (Temp::MemoryManager::Data::Type)0>;
  // extern template struct Temp::DynamicArray<std::thread, (Temp::MemoryManager::Data::Type)1>;
  // extern template struct Temp::DynamicArray<Temp::SceneObject::Data, (Temp::MemoryManager::Data::Type)0>;
  // extern template struct Temp::Array<Temp::Component::CacheData<int>, 5000, (Temp::MemoryManager::Data::Type)0>;
  extern template struct Temp::Array<int, 5000, (Temp::MemoryManager::Data::Type)0>;
}