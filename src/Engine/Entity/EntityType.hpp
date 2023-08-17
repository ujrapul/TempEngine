// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Logger.hpp"
#include "TextBox.hpp"
#include "TextButton.hpp"
#include "Sprite.hpp"
#include "Null.hpp"
#include <string>
#include <thread>

namespace Temp::Mock
{
  struct Data
  {
    Entity::id entity;
  };

  inline void NoOp(int) {}
  inline void DrawConstruct(Scene::Data&, Data&, int shaderType = 0) { NoOp(shaderType); }
  inline void Construct(Scene::Data &, Data &, Entity::id) {}
  inline void DrawDestruct(Scene::Data &, Data &) {}
  inline void Destruct(Scene::Data &, Data &) {}
  inline void Update(Scene::Data &, Data &, float) { std::this_thread::sleep_for(std::chrono::nanoseconds(1)); }
  inline void UpdateRender(Scene::Data &, Data &) {}
}

namespace Temp::EntityType
{
  enum EntityType
  {
    TEXTBOX = 0,
    TEXTBUTTON,
    SPRITE,
    MOCK,
    MAX
  };

  template <uint8_t> struct MapToType_t { using type = NullObject::Data; };
  template <> struct MapToType_t<EntityType::TEXTBOX> { using type = TextBox::Data; };
  template <> struct MapToType_t<EntityType::TEXTBUTTON> { using type = TextButton::Data; };
  template <> struct MapToType_t<EntityType::SPRITE> { using type = Sprite::Data; };
  template <> struct MapToType_t<EntityType::MOCK> { using type = Mock::Data; };

  template <uint8_t T> using MapToType = typename MapToType_t<T>::type;
  
  template <uint8_t> struct MapToCtorType_t { using type = Null; };
  template <> struct MapToCtorType_t<EntityType::TEXTBOX> { using type = TextBox::ConstructData; };
  template <> struct MapToCtorType_t<EntityType::TEXTBUTTON> { using type = TextButton::ConstructData; };
  template <> struct MapToCtorType_t<EntityType::SPRITE> { using type = Sprite::ConstructData; };

  template <uint8_t T> using MapToCtorType = typename MapToCtorType_t<T>::type;
}
