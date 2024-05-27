// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Array.hpp" // IWYU pragma: keep

namespace Temp
{
  template struct DynamicArray<float, MemoryManager::Data::TEMP>;
  template struct DynamicArray<float, MemoryManager::Data::GLOBAL_ARENA>;
  template struct DynamicArray<float, MemoryManager::Data::SCENE_ARENA>;
  template struct DynamicArray<float, MemoryManager::Data::THREAD_TEMP>;
  template struct DynamicArray<char const*, (Temp::MemoryManager::Data::Type)1>;
  template struct DynamicArray<DynamicArray<char const*, (MemoryManager::Data::Type)1>, (MemoryManager::Data::Type)1>;
  template struct Temp::DynamicArray<unsigned int, (Temp::MemoryManager::Data::Type)0>;
  template struct Temp::DynamicArray<unsigned long, (Temp::MemoryManager::Data::Type)0>;
  template struct Temp::DynamicArray<bool, (Temp::MemoryManager::Data::Type)0>;
  // template struct Temp::DynamicArray<std::thread, (Temp::MemoryManager::Data::Type)1>;
  // template struct Temp::DynamicArray<Temp::SceneObject::Data, (Temp::MemoryManager::Data::Type)0>;
  // template struct Temp::Array<Temp::Component::CacheData<int>, 5000, (Temp::MemoryManager::Data::Type)0>;
  template struct Temp::Array<int, 5000, (Temp::MemoryManager::Data::Type)0>;
}