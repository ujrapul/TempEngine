// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "ComponentType.hpp"
#include "Entity.hpp"
#include "EntityData.hpp"
#include "UT_Common.hpp"

namespace Temp::Entity::UnitTests
{
  inline void Run()
  {
    Entity::Data entityData;
    Entity::Init(entityData);

    AssertEqual("Test Entity Create id 0", Entity::Create(entityData), (unsigned short)0);
    AssertEqual("Test Entity Create id 1", Entity::Create(entityData), (unsigned short)1);
    AssertEqual("Test Entity Create id Living Count", Entity::Count(entityData), 2ul);

    Entity::Destroy(entityData, 1);
    AssertEqual("Test Entity Destroy Entity", Entity::Count(entityData), 1ul);

    Entity::id entity;
    for (int i = 0; i < 100; ++i)
    {
      entity = Entity::Create(entityData);
    }

    Entity::AddComponent<Component::Type::TEXT>(entityData, entity, "Test");

    Assert("Test Entity Add Component and ComponentBits",
           Test(Entity::ComponentBits(entityData, entity), Component::Type::TEXT));

    Entity::Reset(entityData);

    AssertEqual("Test Entity Reset", Entity::Count(entityData), 0ul);

    Entity::Destruct(entityData);
  }
}