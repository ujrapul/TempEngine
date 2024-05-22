// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Component.hpp"
#include "ComponentData.hpp"
#include "ComponentType.hpp"
#include "MemoryManager.hpp"
#include <cstdint>

namespace Temp::Component::Container
{
  constexpr uint8_t MAX = sizeof(ComponentBits) * 8; // Representing number of bits

  struct Data
  {
    void* components[MAX];
  };

  template <uint8_t T>
  constexpr void Init(Data& data)
  {
    data.components[T] = static_cast<ArrayData<MapToComponentDataType<T>>*>(
      MemoryManager::data.Allocate(MemoryManager::Data::Type::SCENE_ARENA,
                                   sizeof(ArrayData<MapToComponentDataType<T>>)));
    Temp::Component::Init(
      *static_cast<Temp::Component::ArrayData<MapToComponentDataType<T>>*>(data.components[T]));
  }

  template <uint8_t T>
  constexpr void Destruct(Data& data)
  {
    data.components[T] = nullptr;
  }

  template <uint8_t T>
  constexpr void Reset(Data& data)
  {
    auto* arrayData = static_cast<ArrayData<MapToComponentDataType<T>>*>(data.components[T]);
    Component::Reset(*arrayData);
  }

  template <uint8_t T>
  [[nodiscard]] constexpr ArrayData<MapToComponentDataType<T>>& GetComponentArray(Data& data)
  {
    return *static_cast<Temp::Component::ArrayData<MapToComponentDataType<T>>*>(data.components[T]);
  }

  template <uint8_t T>
  [[nodiscard]] constexpr MapToComponentDataType<T>& Get(Data& data, Entity::id entity)
  {
    // This mess is only needed to make sure we don't need to reference the type twice
    // in the function (once for enum and another for assigned type)
    return Component::Get(GetComponentArray<T>(data), entity);
  }

  template <uint8_t T>
  constexpr void Set(Data& data, Entity::id entity, MapToComponentDataType<T> component)
  {
    Component::Set(GetComponentArray<T>(data), entity, std::move(component));
  }

  template <uint8_t T>
  constexpr void SetCache(Data& data, Entity::id entity)
  {
    Component::SetCache(GetComponentArray<T>(data), entity);
  }

  template <uint8_t T>
  [[nodiscard]] MapToComponentDataType<T> GetCache(Data& data, Entity::id entity)
  {
    return Component::GetCache(GetComponentArray<T>(data), entity);
  }

  template <uint8_t T>
  constexpr void Remove(Data& data, Entity::id entity)
  {
    Component::Remove(GetComponentArray<T>(data), entity);
  }

  void Init(Data& data);
  void Destruct(Data& data);
  void EntityDestroyed(Data& data, Entity::id entity);
  void Reset(Data& data);
}

namespace Temp
{
  extern template struct Array<Component::Null, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  extern template struct Array<Entity::id, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  extern template struct Array<std::size_t, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  extern template struct Array<Component::CacheData<Component::Null>, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;

  extern template struct Array<Math::Vec2f, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  extern template struct Array<Component::CacheData<Math::Vec2f>, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;

  extern template struct Array<Component::Drawable::Data, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  extern template struct Array<Component::CacheData<Component::Drawable::Data>, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;

  extern template struct Array<SceneString, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  extern template struct Array<Component::CacheData<SceneString>, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;

  extern template struct Array<Component::Hoverable::Data, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  extern template struct Array<Component::CacheData<Component::Hoverable::Data>, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;

  extern template struct Array<Component::Updateable::Data, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  extern template struct Array<Component::CacheData<Component::Updateable::Data>, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;

  extern template Temp::Math::Vec2<float> Temp::Component::dummy<Temp::Math::Vec2<float>>;
  extern template Temp::BaseString<Temp::MemoryManager::Data::Type::SCENE_ARENA> Temp::Component::dummy<Temp::BaseString<Temp::MemoryManager::Data::Type::SCENE_ARENA>>;
  extern template Temp::Component::Drawable::Data Temp::Component::dummy<Temp::Component::Drawable::Data>;
  extern template Temp::Component::Hoverable::Data Temp::Component::dummy<Temp::Component::Hoverable::Data>;
}
