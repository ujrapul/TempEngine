// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array.hpp"
#include "Camera.hpp"
#include "Hoverable.hpp"
#include "Scene.hpp"
#include "UT_Common.hpp"

namespace Temp::Component::Hoverable::UnitTests
{
  inline bool isHoverEnter = false;
  inline bool isHoverLeave = false;
  inline bool isDrag = false;

  inline void ResetBools()
  {
    isHoverEnter = false;
    isHoverLeave = false;
    isDrag = false;
  }

  inline void HoverEnter(Scene::Data&, Hoverable::Data&) { isHoverEnter = true; }
  inline void HoverLeave(Scene::Data&, Hoverable::Data&) { isHoverLeave = true; }
  inline void Drag(Scene::Data&, Hoverable::Data&, float, float) { isDrag = true; }

  inline void Run()
  {
    Camera::SetProjection(Camera::Projection::ORTHOGRAPHIC);

    Scene::Data* scene = new Scene::Data();
    Data hoverable{nullptr, HoverEnter, HoverLeave, nullptr, nullptr, -1000, -1000, 2000, 2000};
    Assert("Test Hoverable is Inside", Hoverable::IsInside(hoverable, 0, 0));
    Assert("Test Hoverable is not Inside", !Hoverable::IsInside(hoverable, -1000, -1000));

    Hoverable::HoverableEnter(*scene, hoverable);
    Assert("Test Hoverable HoverableEnter", isHoverEnter && hoverable.lastInside);

    Hoverable::HoverableLeave(*scene, hoverable);
    Assert("Test Hoverable HoverableEnter", isHoverLeave && !hoverable.lastInside);

    DynamicArray<DynamicArray<Math::Vec3f, MemoryManager::Data::SCENE_ARENA>,
                 MemoryManager::Data::SCENE_ARENA>
      triangles = {
        {
          {0.f, 2.0f, 0.0},
          {-2.f, -2.f, 0.0},
          {2.f, -2.f, 0.0},
        },
      };
    Component::Hoverable::Data hoverable2 = {
      .Click = nullptr,
      .HoverEnter = HoverEnter,
      .HoverLeave = HoverLeave,
      .Drag = Drag,
      .x = -9999,
      .y = -9999,
      .width = 0,
      .height = 0,
      .scale = {1.f, 1.f},
      .model = Math::Mat4{},
      .triangles = triangles,
    };

    Assert("Test Hoverable is Inside Raycast",
           Hoverable::IsInsideRaycast(hoverable2,
                                      (float)Camera::GetWidth() / 2 - 1.f,
                                      (float)Camera::GetHeight() / 2 + 1.f));

    Component::Hoverable::Data hoverable3 = {
      .Click = nullptr,
      .HoverEnter = HoverEnter,
      .HoverLeave = HoverLeave,
      .Drag = Drag,
      .x = 0,
      .y = 0,
      .width = 0,
      .height = 0,
      .scale = {1.f, 1.f},
    };

    ResetBools();

    Hoverable::Drag(*scene, hoverable3, 15.f, 15.f, 50.f, 50.f);
    Assert("Test Hoverable no Drag when Drag flag not set", !isDrag);

    hoverable3.isDrag = true;

    Hoverable::Drag(*scene, hoverable3, 15.f, 15.f, 50.f, 50.f);
    Assert("Test Hoverable Drag when Drag flag set", isDrag);
    AssertEqual("Test Hoverable Drag x", hoverable3.x, 35.f);
    AssertEqual("Test Hoverable Drag y", hoverable3.y, -35.f);

    delete scene;
  }
}
