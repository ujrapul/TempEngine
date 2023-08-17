// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

// Built with reference from Austin Morlan's ECS
//
// Original work Copyright © 2020 Austin Morlan. All rights reserved.
// Modified work Copyright © 2023 Ujwal Vujjini. All rights reserved.
//
// MIT License
//
// Copyright (c) 2020 Austin Morlan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice (including the next paragraph)
// shall be included in all copies or substantial portions of the
// Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
// OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// https://austinmorlan.com/posts/entity_component_system/
// https://code.austinmorlan.com/austin/2019-ecs

#pragma once

#include "Entity.hpp"
#include "Logger.hpp"
#include "Math.hpp"
#include <array>
#include <cassert>
#include <climits>
#include <mutex>

namespace Temp
{
  namespace Component
  {
    template <typename T>
    inline T dummy{};

    template <typename T>
    struct ArrayData
    {
      // This is using Indexes not Entities!
      std::array<T, Entity::MAX> array{};
      // Value = Entity::id | Index = Data Index
      std::array<Entity::id, Entity::MAX> sparseEntities{};
      // Value = Data Index | Index = Entity
      std::array<std::size_t, Entity::MAX> sparseIndices{};
      std::size_t size{};
    };

    template <typename T>
    inline void Reset(ArrayData<T>& data)
    {
      // NOTE: Shouldn't be necessary
      data.array.fill({});
      Init(data);
    }

    template <typename T>
    inline void Init(ArrayData<T>& data)
    {
      data.sparseEntities.fill({Entity::MAX});
      data.sparseIndices.fill({SIZE_MAX});
      data.size = 0;
    }

    template <typename T>
    inline void Set(ArrayData<T>& data, Entity::id entity, const T& component)
    {
      assert(entity < Entity::MAX && "[ComponentData] Set: Entity::id not valid!");

      if (data.sparseIndices[entity] < Entity::MAX)
      {
        data.array[data.sparseIndices[entity]] = component;
      }
      else
      {
        std::size_t newIndex = data.size;
        data.sparseIndices[entity] = newIndex;
        data.sparseEntities[newIndex] = entity;
        data.array[newIndex] = component;
        ++data.size;
      }
    }

    template <typename T>
    inline void Remove(ArrayData<T>& data, Entity::id entity)
    {
      assert(entity < Entity::MAX && "[ComponentData] Remove: Entity::id not valid!");

      std::size_t indexOfRemovedEntity = data.sparseIndices[entity];
      std::size_t indexOfLastElement = data.size - 1;
      data.array[indexOfRemovedEntity] = data.array[indexOfLastElement];

      Entity::id entityOfLastElement = data.sparseEntities[indexOfLastElement];
      data.sparseIndices[entityOfLastElement] = indexOfRemovedEntity;
      data.sparseEntities[indexOfRemovedEntity] = entityOfLastElement;

      data.sparseIndices[entity] = SIZE_MAX;
      data.sparseEntities[indexOfLastElement] = Entity::MAX;

      --data.size;

      data.array[data.size] = {};
    }

    template <typename T>
    [[nodiscard]] constexpr const T& Get(const ArrayData<T>& data, Entity::id entity)
    {
      assert(entity < Entity::MAX && "[ComponentData] Get const: Entity::id not valid!");

      if (data.sparseIndices[entity] < Entity::MAX)
      {
        return data.array[data.sparseIndices[entity]];
      }
      Logger::Log("[ComponentData] Get const: Warning! Accessing invalid entity!");
      return dummy<T>;
    }

    template <typename T>
    [[nodiscard]] constexpr T& Get(ArrayData<T>& data, Entity::id entity)
    {
      assert(entity < Entity::MAX && "[ComponentData] Get: Entity::id not valid!");

      if (data.sparseIndices[entity] < Entity::MAX)
      {
        return data.array[data.sparseIndices[entity]];
      }
      Logger::Log("[ComponentData] Get: Warning! Accessing invalid entity!");
      return dummy<T>;
    }

    template <typename T>
    inline void EntityDestroyed(ArrayData<T>& data, Entity::id entity)
    {
      if (data.sparseIndices[entity] < Entity::MAX)
      {
        Remove<T>(data, entity);
      }
    }
  }
}
