// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Entity.hpp"
#include "Shader.hpp"

namespace Temp::Scene
{
  struct Data;
}

namespace Temp::EditorGrid
{
  struct Data
  {
    Entity::id entity{};
  };

  void Construct(Scene::Data& scene, Data& editorGrid);
#ifdef EDITOR
  void DrawConstruct(Scene::Data& scene, Data& editorGrid, int shaderType = Render::EditorShaderIdx::EDITORGRID);
#else
  void DrawConstruct(Scene::Data& scene, Data& editorGrid, int shaderType = 0);
#endif
  void DrawUpdate(Scene::Data& scene, Data& editorGrid);
  void DrawDestruct(Scene::Data& scene, Data& editorGrid);
  void Destruct(Scene::Data& scene, Data& editorGrid);
}