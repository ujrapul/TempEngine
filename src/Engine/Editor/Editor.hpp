// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

namespace Temp
{
  namespace Engine
  {
    struct Data;
  }

  namespace Scene
  {
    struct Data;
  }

  namespace SceneObject
  {
    struct Data;
  }

  namespace Event
  {
    struct Data;
  }

  namespace Editor
  {
    void RunImGui(Scene::Data &scene, Event::Data& EventData);
  }
}