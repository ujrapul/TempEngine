// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

//  Built with reference from Austin Morlan's ECS
//
//  Original work Copyright © 2020 Austin Morlan. All rights reserved.
//  Modified work Copyright © 2023 Ujwal Vujjini. All rights reserved.
//
//  MIT License
//
//  Copyright (c) 2020 Austin Morlan
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice (including the next paragraph)
//  shall be included in all copies or substantial portions of the
//  Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
//  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
//  OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//  https://austinmorlan.com/posts/entity_component_system/
//  https://code.austinmorlan.com/austin/2019-ecs

#pragma once

#include "Components/ComponentContainer.hpp"
#include "Components/ComponentType.hpp"
#include "EntityManager.hpp"

namespace Temp::Coordinator
{
  struct Data
  {
    EntityManager::Data entityData;
    Component::Container::Data componentData;
  };

  [[nodiscard]] Entity CreateEntity(Data &coordinator);
  void DestroyEntity(Data &coordinator, Entity entity);
  void Init(Data &coordinator);
  void Destruct(Data &coordinator);
  void Reset(Data &coordinator);
  
  inline const Signature& GetSignature(Data &coordinator, Entity entity)
  {
    return EntityManager::GetSignature(coordinator.entityData, entity);
  }

  template <uint8_t T>
  constexpr void AddComponent(Data &coordinator, Entity entity, const Component::MapToComponentDataType<T>& component)
  {
    Signature sig = EntityManager::GetSignature(coordinator.entityData, entity);
    sig.set(T);
    EntityManager::SetSignature(coordinator.entityData,
                                entity,
                                sig);
    Component::Container::Set<T>(coordinator.componentData,
                                 entity,
                                 component);
  }

  template <uint8_t T>
  constexpr void RemoveComponent(Data &coordinator, Entity entity)
  {
    Signature sig = EntityManager::GetSignature(coordinator.entityData, entity);
    sig.set(T, false);
    EntityManager::SetSignature(coordinator.entityData,
                                entity,
                                sig);
    Component::Container::Remove<T>(coordinator.componentData,
                                 entity);
  }
}
