// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "STDPCH.hpp"
#include "Entity.hpp"
#include "Shader.hpp"
#include "Math.hpp"

namespace Temp::Scene
{
  struct Data;
}

namespace Temp::Component::Drawable
{
  struct Data;
}

namespace Temp::Sprite
{
  struct ConstructData
  {
    Math::Vec2f pos{};
    Math::Vec2f scale{};
  };

  struct Data
  {
    SceneString fileName{};
    Entity::id entity{};
  };

  void Construct(Scene::Data& scene, Data& sprite, ConstructData& ctorData, Entity::id entity);
  void DrawConstruct(Scene::Data& scene, Data& sprite, int shaderType = Render::ShaderIdx::SPRITE);
  // void DrawUpdate(Scene::Data& scene, Component::Drawable::Data& drawable, Data& sprite);
  void DrawDestruct(Scene::Data& scene, Data& sprite);
  void Destruct(Scene::Data& scene, Data& sprite);
}
