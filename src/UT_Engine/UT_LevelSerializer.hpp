// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Scene.hpp"
#include "SceneObject.hpp"
#include "UT_Common.hpp"
#include "LevelSerializer.hpp"
#include "TextBox.hpp"
#include "TextButton.hpp"
#include "Math.hpp"
#include "Entity.hpp"
#include "Engine.hpp"

namespace Temp::LevelSerializer::UnitTests
{
  inline void TestDeserialize(Scene::Data &scene, const std::string &testName, const char* file)
  {
    Assert(testName + " Test Level Parsing", Deserialize(scene, file));

    auto &objects = scene.objects;
    auto &table = scene.objectsNameIdxTable;

    AssertEqual(testName + " Test Level Parsing objects size after Parse", (int)objects.size, 2);
    auto *object0 = static_cast<TextBox::Data *>(objects[0].data);
    auto *object1 = static_cast<TextButton::Data *>(objects[1].data);
    auto &subObject10 = object1->textBox;
    auto &constructData0 = *static_cast<TextBox::ConstructData *>(objects[0].constructData);
    auto &constructData1 = *static_cast<TextButton::ConstructData *>(objects[1].constructData);
    for (size_t i = 0; i < objects.size; ++i)
    {
      objects[i].entity = Scene::CreateEntity(scene);
      SceneObject::Construct(scene, objects[i]);
    }
    AssertEqual(testName + " Test Level Parsing object 0 valid (1 type)", objects[0].type, (int)EntityType::TEXTBOX);
    AssertEqual(testName + " Test Level Parsing object 0 valid (2 name)", objects[0].name, {"NumberGameTextBox"});
    AssertEqual(testName + " Test Level Parsing object 0 valid (3 text)", object0->text, {"NumberGame"});
    AssertEqual(testName + " Test Level Parsing object 0 valid (4 x)", constructData0.x, 0.f);
    AssertEqual(testName + " Test Level Parsing object 0 valid (5 y)", constructData0.y, 40.f);
    AssertEqual(testName + " Test Level Parsing object 0 valid (6 scale)", constructData0.scale, 0.04f);
    AssertEqual(testName + " Test Level Parsing object 0 valid (7 table)", objects[table[objects[0].name.c_str()]].data, objects[0].data);

    AssertEqual(testName + " Test Level Parsing object 1 valid (1 type)", objects[1].type, (int)EntityType::TEXTBUTTON);
    AssertEqual(testName + " Test Level Parsing object 1 valid (2 name)", objects[1].name, {"NumberGameTextButton"});
    AssertEqual(testName + " Test Level Parsing object 1 valid (3 text)", subObject10.text, {"NumberGame1"});
    AssertEqual(testName + " Test Level Parsing subObject 10 valid (4 x)", Scene::Get<Component::Type::POSITION2D>(scene, subObject10.entity).x, -678.f);
    AssertEqual(testName + " Test Level Parsing subObject 10 valid (5 y)", Scene::Get<Component::Type::POSITION2D>(scene, subObject10.entity).y, 50.f);
    AssertEqual(testName + " Test Level Parsing subObject 10 valid (6 scale)", Scene::Get<Component::Type::SCALE>(scene, subObject10.entity).x, 0.0345f);
    AssertEqual(testName + " Test Level Parsing subObject 10 valid (7 x)", constructData1.hoverable.x, -20.f);
    AssertEqual(testName + " Test Level Parsing constructData 1 valid (8 y)", constructData1.hoverable.y, 0.f);
    AssertEqual(testName + " Test Level Parsing constructData 1 valid (9 width)", constructData1.hoverable.width, 9.f);
    AssertEqual(testName + " Test Level Parsing constructData 1 valid(10 height)", constructData1.hoverable.height, 4.f);
    AssertEqual(testName + " Test Level Parsing constructData 1 valid(10 table)", objects[table[objects[1].name.c_str()]].data, objects[1].data);
  }

  inline void Run()
  {
    SceneObject::Init();
    Scene::Data scene;
    Entity::Init(scene.entityData);
    scene.sceneFns->name = "SerializeTest";

    TestDeserialize(scene, "Deserialize", "Test.level");
    Serialize(scene, (AssetsDirectory() / "Levels" / "SerializeTest.level").c_str());
    CleanupScene(scene);
    
    TestDeserialize(scene, "Serialize", "SerializeTest.level");
    CleanupScene(scene);
    Entity::Destruct(scene.entityData);
  }
}
