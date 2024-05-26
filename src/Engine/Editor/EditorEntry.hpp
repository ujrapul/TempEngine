// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Camera.hpp"
#include "EditorLevel.hpp"
#include "Engine.hpp"
#include "Math.hpp"

namespace Temp::EditorEntry
{
  struct Data
  {
    Math::Vec4f backgroundColor{0.2f, 0.2f, 0.2f, 1.0f};
  };

  inline void Run(const char* windowName, const Data& data)
  {
    using namespace std;

    Camera::SetProjection(Camera::Projection::ORTHOGRAPHIC);

    Temp::Engine::Data engine;
    engine.backgroundColor = data.backgroundColor;

    Scene::SceneFns scene = Scene::EditorLevel::Create();
    engine.sceneFns.PushBack(scene);

    Global::Construct(engine);
    Global::Run(windowName, 1920, 1080);
  }
}
