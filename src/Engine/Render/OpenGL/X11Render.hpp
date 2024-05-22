// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

namespace Temp::Scene
{
  struct Data;
}

namespace Temp::Render
{
  void Initialize(Scene::Data& scene, const char *windowName, int windowX, int windowY);
  void Run(Scene::Data& scene);
  void Destroy();
}
