// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

namespace Temp::Event
{
  struct Data;
}

namespace Temp::Scene
{
  struct Data;
}

namespace Temp::Render
{
#ifdef EDITOR
  void RenderImGui(Scene::Data& scene, Event::Data& EventData);
#endif
}