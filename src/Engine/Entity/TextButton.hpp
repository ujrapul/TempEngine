// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Entity.hpp"
#include "Hoverable.hpp"
#include "Shader.hpp"
#include "TextBox.hpp"

namespace Temp::Scene
{
  struct Data;
}

namespace Temp::Component::Hoverable
{
  struct Data;
}

namespace Temp::TextButton
{
  struct ConstructData
  {
    Component::Hoverable::Data hoverable;
    TextBox::ConstructData textBoxCtorData;
  };

  struct Data
  {
    TextBox::Data textBox{};
    void *callbackData{nullptr};
    Entity::id entity{};
  };

  void Construct(Scene::Data& scene, Data& textButton, ConstructData& ctorData, Entity::id entity);
  void DrawConstruct(Scene::Data& scene, Data& textButton, int shaderType = Render::ShaderIdx::TEXT);
  void DrawUpdate(Scene::Data& scene, Component::Drawable::Data& drawable, Data& textButton);
  void UpdateText(Scene::Data& scene, Data& textButton, const char* newText);

  void DrawDestruct(Scene::Data& scene, Data& textButton);
  void Destruct(Scene::Data& scene, Data& textButton);
  void HoverEnter(Temp::Scene::Data& scene, Temp::Component::Hoverable::Data& hoverable);
  void HoverLeave(Temp::Scene::Data& scene, Temp::Component::Hoverable::Data& hoverable);
  void SetHoverableCallbacks(
    Scene::Data& scene,
    Data& textButton,
    void (*Click)(Scene::Data&, Component::Hoverable::Data&) = [](auto&, auto&) {},
    void (*HoverEnter)(Scene::Data&, Component::Hoverable::Data&) = TextButton::HoverEnter,
    void (*HoverLeave)(Scene::Data&, Component::Hoverable::Data&) = TextButton::HoverLeave);
  void SetHoverableCallbacks(
    Scene::Data& scene,
    Data& textButton,
    void* callbackData,
    void (*Click)(Scene::Data&, Component::Hoverable::Data&) = [](auto&, auto&) {},
    void (*HoverEnter)(Scene::Data&, Component::Hoverable::Data&) = TextButton::HoverEnter,
    void (*HoverLeave)(Scene::Data&, Component::Hoverable::Data&) = TextButton::HoverLeave);

  void Scale(Scene::Data& scene, Data& textButton, float scale);
  void SetVisiblity(Scene::Data& scene, Data& textButton, bool visible);
}
