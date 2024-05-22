// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Entity.hpp"
#include "Logger.hpp"

struct Null
{
};

namespace Temp::Scene
{
  struct Data;
}

namespace Temp::NullObject
{
  struct Data
  {
  };

  inline void NoOp(int) {}
  inline void DrawConstruct(Scene::Data&, Data&, int shaderType = 0) { NoOp(shaderType); }
  inline void Construct(Scene::Data&, Data&, Entity::id) {}
  inline void DrawDestruct(Scene::Data&, Data&) {}
  inline void Destruct(Scene::Data&, Data&) {}
  inline void Update(Scene::Data&, Data&, float) {}
  inline void UpdateRender(Scene::Data&, Data&) {}
}