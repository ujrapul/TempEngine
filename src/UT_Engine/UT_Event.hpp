// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Camera.hpp"
#include "Event.hpp"
#include "Hoverable.hpp"
#include "Math.hpp"
#include "UT_Common.hpp"

namespace Temp::Event::UnitTests
{
  inline bool isHoverEnter = false;
  inline bool isHoverLeave = false;
  inline bool isButtonReleased = false;

  inline void ResetBools()
  {
    isHoverEnter = false;
    isHoverLeave = false;
    isButtonReleased = false;
  }

  inline void HoverEnter(Scene::Data&, Component::Hoverable::Data&) { isHoverEnter = true; }
  inline void HoverLeave(Scene::Data&, Component::Hoverable::Data&) { isHoverLeave = true; }
  inline void ButtonReleased(Scene::Data&, Component::Hoverable::Data&) { isButtonReleased = true; }

  inline void Run()
  {
    Camera::SetProjection(Camera::Projection::ORTHOGRAPHIC);

    Scene::Data scene;
    Scene::Construct(scene);
    Entity::Init(scene.entityData);

    Event::Data EventData;

    Event::Hover(scene, EventData, 0, 0);
    Event::ButtonReleased(scene, EventData, 0, 0, 1);

    Assert("Test Event Hover Terminates with non Scene State Run", !isHoverEnter && !isHoverLeave);
    Assert("Test Event ButtonReleased Terminates with non Scene State Run", !isButtonReleased);

    scene.state = Scene::State::RUN;

    Event::Hover(scene, EventData, 0, 0);
    Event::ButtonReleased(scene, EventData, 0, 0, 1);

    Assert("Test Event Hover Terminates with No Hoverable", !isHoverEnter && !isHoverLeave);
    Assert("Test Event ButtonReleased Terminates with No Hoverable", !isButtonReleased);

    Scene::AddComponent<Component::Type::HOVERABLE>(scene, 0, {});

    Event::Hover(scene, EventData, 0, 0);
    Event::ButtonReleased(scene, EventData, 0, 0, 1);

    Assert("Test Event Hover Terminates with No Hoverable Callback",
           !isHoverEnter && !isHoverLeave);
    Assert("Test Event ButtonReleased Terminates with No Hoverable Callback", !isButtonReleased);

    Component::Hoverable::Data hoverable = {
      .Click = ButtonReleased,
      .HoverEnter = HoverEnter,
      .HoverLeave = HoverLeave,
      .x = -1280,
      .y = -720,
      .width = 9999,
      .height = 9999,
      .scale = {1.f, 1.f},
    };
    Scene::AddComponent<Component::Type::HOVERABLE>(scene, 0, hoverable);

    Event::Hover(scene, EventData, 0, 0);
    Assert("Test Event Hover Enter when Mouse in Hoverable", isHoverEnter && !isHoverLeave);

    hoverable.lastInside = true;
    Event::ButtonPressed(scene, EventData, 1, 0, 1);
    Assert("Test Event ButtonPressed when Mouse in Hoverable Draggable is valid",
           EventData.draggable);
    AssertEqual("Test Event ButtonPressed when Mouse in Hoverable Draggable is hoverable",
                *EventData.draggable,
                hoverable);

    Event::ButtonReleased(scene, EventData, 0, 0, 1);
    Assert("Test Event ButtonReleased when Mouse in Hoverable", isButtonReleased);

    ResetBools();

    Component::Hoverable::Data hoverable1 = {
      ButtonReleased,
      HoverEnter,
      HoverLeave,
      nullptr,
      nullptr,
      -9999,
      -9999,
      0,
      0,
      {1.f, 1.f},
      true,
    };

    Scene::AddComponent<Component::Type::HOVERABLE>(scene, 0, hoverable1);

    Event::Hover(scene, EventData, 0, 0);
    Event::ButtonReleased(scene, EventData, 0, 0, 1);

    Assert("Test Event Hover Leave when Mouse is not in Hoverable", !isHoverEnter && isHoverLeave);
    Assert("Test Event ButtonReleased when Mouse is not in Hoverable", !isButtonReleased);

    DynamicArray<DynamicArray<Math::Vec3f, MemoryManager::Data::SCENE_ARENA>,
                 MemoryManager::Data::SCENE_ARENA>
      triangles = {
        {
          {1.0f, 1.0f, 0.0},
          {-1.f, -1.f, 0.0},
          {1.f, -1.f, 0.0},
        },
      };
    Component::Hoverable::Data hoverable2 = {
      .Click = ButtonReleased,
      .HoverEnter = HoverEnter,
      .HoverLeave = HoverLeave,
      .x = -50,
      .y = -50,
      .width = 0,
      .height = 0,
      .scale = {1.f, 1.f},
      .model = Math::Mat4{},
      .triangles = triangles,
    };
    Scene::AddComponent<Component::Type::HOVERABLE>(scene, 0, hoverable2);

    isHoverLeave = false;
    Event::Hover(scene, EventData, Camera::GetWidth() / 2, Camera::GetHeight() / 2);
    Event::ButtonReleased(scene, EventData, Camera::GetWidth() / 2, Camera::GetHeight() / 2, 1);

    Assert("Test Event Hover Enter when Mouse in Hoverable Raycast", isHoverEnter && !isHoverLeave);
    Assert("Test Event ButtonReleased when Mouse is not in Hoverable Raycast", isButtonReleased);

    Scene::Destruct(scene);
    Entity::Destruct(scene.entityData);
  }
}
