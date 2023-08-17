// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Engine.hpp"
#include "Math.hpp"

namespace Temp::Render
{
  void Initialize(Scene::Data& scene, const char *windowName, int windowX, int windowY);
  void Run(Scene::Data& scene, const char *windowName, int windowX, int windowY);
  void Run(Scene::Data& scene);
  void Destroy();
  Math::Vec2f GetWindowOrigin();
  Math::Vec2f GetWindowSize();
  Math::Vec2f GetScreenSize();
  
}
