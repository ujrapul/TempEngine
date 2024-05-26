// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "STDPCH.hpp"
#include "Entity.hpp"
#include "Shader.hpp"
#include "Math.hpp"
#include "String.hpp"
#include <cstdint>

namespace Temp::Scene
{
  struct Data;
}

namespace Temp::Component::Drawable
{
  struct Data;
}

// NOTE: Don't use inline global mutexes, it'll stall multiple instances of the same object
namespace Temp::TextBox
{
  struct ConstructData
  {
    float x{};
    float y{};
    float scale{};
    // Needed for Hoverable in TextButton
    float offsetY{0};
    // Used for reserving memory
    size_t maxCharacters{SIZE_MAX};
  };

  struct Data
  {
    SceneString text{};
    Math::Vec2f size{};
    size_t maxCharacters{SIZE_MAX};
    Entity::id entity{};
    int fontType{};
    int maxCharactersPerLine{INT_MAX};
    bool enableOutline{false};
    bool hasParent{false};
    bool singleCharacterWriteMode{false};
  };

  void DrawConstruct(Scene::Data& scene, Data& textBox, int shaderType = Render::ShaderIdx::TEXT);
  void Construct(Scene::Data& scene, Data& textBox, const ConstructData& ctorData, Entity::id entity);
  void UpdateText(Scene::Data& scene, Data& textBox, const char* newText);
  void Update(Scene::Data& scene, Data& textBox);
  void DrawUpdate(Scene::Data& scene, Component::Drawable::Data& drawable, Data& textBox);
  void EnableOutline(Scene::Data& scene, Data& textBox, bool enable);
  void DrawDestruct(Scene::Data& scene, Data& textBox);
  void Destruct(Scene::Data& scene, Data& textBox);
  String FormatText(Data& textBox, String& text, String& sectionBreak);
}
