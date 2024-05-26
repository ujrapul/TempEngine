// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Component.hpp"
#include "ComponentContainer.hpp"
#include "ComponentType.hpp"
#include "Entity.hpp"

namespace Temp
{
  namespace Entity
  {
    struct Data
    {
      // Using dynamic array so it isn't initialized on construction
      // Should be used as a regular array here!
      SceneDynamicArray<bool> used{};
      SceneDynamicArray<::Temp::ComponentBits> componentBits{};
      Component::Container::Data componentContainer;
    };

    void Init(Data& data);
    void Destroy(Data& data, Entity::id entity);
    [[nodiscard]] Entity::id Create(Data& data);
    [[nodiscard]] ComponentBits& ComponentBits(Data& data, Entity::id entity);
    void Destruct(Data& data);
    void Reset(Data& data);
    size_t Count(Data& data);

    template <uint8_t T>
    constexpr void AddComponent(Data& data,
                                Entity::id entity,
                                Component::MapToComponentDataType<T> component)
    {
      SetBit(Entity::ComponentBits(data, entity), T);
      Component::Container::Set<T>(data.componentContainer, entity, std::move(component));
    }

    template <uint8_t T>
    constexpr void RemoveComponent(Data& data, Entity::id entity)
    {
      ClearBit(Entity::ComponentBits(data, entity), T);
      Component::Container::Remove<T>(data.componentContainer, entity);
    }

    template <uint8_t T>
    constexpr void AddCacheComponent(Data& data, Entity::id entity)
    {
      Component::Container::SetCache<T>(data.componentContainer, entity);
      RemoveComponent<T>(data, entity);
    }

    template <uint8_t T>
    constexpr void RemoveCacheComponent(Data& data, Entity::id entity)
    {
      AddComponent<T>(data,
                      entity,
                      Component::Container::GetCache<T>(data.componentContainer, entity));
    }

    template <uint8_t T>
    [[nodiscard]] constexpr Component::MapToComponentDataType<T>& Get(Data& entityData,
                                                                      Entity::id entity)
    {
      return Component::Container::Get<T>(entityData.componentContainer, entity);
    }

    template <uint8_t T>
    [[nodiscard]] constexpr Component::ArrayData<Component::MapToComponentDataType<T>>&
    GetComponentArray(Data& entityData)
    {
      return *static_cast<Component::ArrayData<Component::MapToComponentDataType<T>>*>(
        entityData.componentContainer.components[T]);
    }
  }
}
