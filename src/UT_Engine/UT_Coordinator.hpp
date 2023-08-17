// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "ComponentType.hpp"
#include "Coordinator.hpp"
#include "Entity.hpp"
#include "UT_Common.hpp"

namespace Temp::Coordinator::UnitTests
{
  void Run()
  {
    Coordinator::Data coordinator;
    Coordinator::Init(coordinator);

    AssertEqual("Test Coordinator Create Entity 0",
                Coordinator::CreateEntity(coordinator),
                0u);
    AssertEqual("Test Coordinator Create Entity 1",
                Coordinator::CreateEntity(coordinator),
                1u);
    AssertEqual("Test Coordinator Create Entity Living Count",
                coordinator.entityData.livingEntityCount,
                2u);
    Coordinator::DestroyEntity(coordinator, 1);
    AssertEqual("Test Coordinator Destroy Entity", coordinator.entityData.livingEntityCount, 1u);

    Entity entity;
    for (int i = 0; i < 100; ++i)
    {
      entity = Coordinator::CreateEntity(coordinator);
    }

    Coordinator::AddComponent<Component::Type::TEXT>(coordinator, entity, "Test");

    Assert("Test Coordinator Add Component and Signature",
           Coordinator::GetSignature(coordinator, entity).test(Component::Type::TEXT));

    Coordinator::Reset(coordinator);

    AssertEqual("Test Coordinator Reset", coordinator.entityData.livingEntityCount, 0u);

    Coordinator::Destruct(coordinator);
  }
}