// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "ComponentType.hpp"
#include "EntityType.hpp"
#include "Math.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "TextBox.hpp"
#include "UT_Common.hpp"

namespace Temp::Scene::UnitTests
{
  inline void Run()
  {
    SceneObject::Init();

    Scene::Data scene;
    Entity::Init(scene.entityData);
    SceneObject::Data object{
      .data = new Mock::Data(),
      .constructData = nullptr,
      .name = "Mock0",
      .type = EntityType::MOCK,
      .entity = 6,
    };
    SceneObject::Data object1{
      .data = new Mock::Data(),
      .constructData = nullptr,
      .name = "Mock1",
      .type = EntityType::MOCK,
      .entity = 200,
    };

    for (int i = 0; i < 200; ++i)
    {
      Scene::CreateEntity(scene);
    }
    Scene::AddComponent<Component::Type::POSITION2D>(scene, 0, {543, 54});
    Scene::AddComponent<Component::Type::POSITION2D>(scene, 6, {45, 5476});

    AssertEqual("Test Scene Add & Get Component",
                Scene::Get<Component::Type::POSITION2D>(scene, 0),
                Math::Vec2f{543, 54});

    AssertEqual("Test Scene Get Component for non-existent Entity",
                Scene::Get<Component::Type::POSITION2D>(scene, 999),
                Math::Vec2f{});

    AssertEqual("Test Scene Get Component Array index 0",
                Scene::GetComponentArray<Component::Type::POSITION2D>(scene).array[0],
                Math::Vec2f{543, 54});

    AssertEqual("Test Scene Get Component Array index 6",
                Scene::GetComponentArray<Component::Type::POSITION2D>(scene).array[1],
                Math::Vec2f{45, 5476});

    AssertEqual("Test Scene Get Component Array Size",
                Scene::GetComponentArray<Component::Type::POSITION2D>(scene).size,
                (size_t)2);

    Scene::DestroyEntity(scene, 6);

    AssertEqual("Test Scene Destroy Entity",
                Scene::GetComponentArray<Component::Type::POSITION2D>(scene).array[1],
                Math::Vec2f{});
    AssertEqual("Test Scene Get Component Array Size",
                Scene::GetComponentArray<Component::Type::POSITION2D>(scene).size,
                (size_t)1);

    AssertEqual("Test Scene Get Component Array Size",
                Scene::GetComponentArray<Component::Type::POSITION2D>(scene).size,
                (size_t)1);

    Scene::SpawnObject(scene, object);
    Scene::AddComponent<Component::Type::POSITION2D>(scene, object.entity, {67, 956});
    Scene::AddComponent<Component::Type::SCALE>(scene, object.entity, {1.f, 1.f});

    AssertEqual("Test Scene Spawn Object Get Name", Scene::GetObject(scene, "Mock0"), object);
    AssertEqual("Test Scene Spawn Object Get Entity",
                Scene::GetObject(scene, object.entity),
                object);

    Scene::SpawnObject(scene, object1);
    AssertEqual("Test Scene Spawn Object Get Name 1", Scene::GetObject(scene, "Mock1"), object1);
    AssertEqual("Test Scene Spawn Object Get Entity::id 1",
                Scene::GetObject(scene, object1.entity),
                object1);

    SceneObject::Translate(scene, object, {45, 56, 0});

    AssertEqual("Test Scene Translate Object",
                Scene::Get<Component::Type::POSITION2D>(scene, object.entity),
                Math::Vec2f{45, 56});

    SceneObject::Scale(scene, object, 5.f);

    AssertEqual("Test Scene Scale Object",
                Scene::Get<Component::Type::SCALE>(scene, object.entity),
                {5.f, 5.f});

    SceneObject::Scale(scene, object, {5.f, 7.f});

    AssertEqual("Test Scene Scale Object",
                Scene::Get<Component::Type::SCALE>(scene, object.entity),
                {5.f, 7.f});

    Scene::RemoveObject(scene, object);

    AssertEqual("Test Scene Remove Object Get Name", //
                Scene::GetObject(scene, "Mock0"),
                {});
    AssertEqual("Test Scene Remove Object Get Entity", Scene::GetObject(scene, object.entity), {});

    Scene::RemoveObject(scene, object1);
    Entity::Destruct(scene.entityData);
  }
}