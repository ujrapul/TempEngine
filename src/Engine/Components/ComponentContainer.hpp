// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Component.hpp"
#include "ComponentData.hpp"
#include "ComponentType.hpp"

#include <cstdint>

namespace Temp::Component::Container
{
  constexpr uint8_t MAX = sizeof(ComponentBits) * 8; // Representing number of bits

  struct Data
  {
    std::array<void*, MAX> components{};
  };

  template <uint8_t T>
  constexpr void Init(Data& data)
  {
    data.components[T] = new ArrayData<MapToComponentDataType<T>>();
    Temp::Component::Init(
      *static_cast<Temp::Component::ArrayData<MapToComponentDataType<T>>*>(data.components[T]));
  }

  template <uint8_t T>
  constexpr void Destruct(Data& data)
  {
    delete static_cast<ArrayData<MapToComponentDataType<T>>*>(data.components[T]);
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
  constexpr void Set(Data& data, Entity::id entity, const MapToComponentDataType<T>& component)
  {
    Component::Set(GetComponentArray<T>(data), entity, component);
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
