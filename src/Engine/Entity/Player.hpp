// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Math.hpp"
#include "Entity.hpp"
#include <string>

namespace Temp
{
  namespace Player2D
  {
    struct Data
    {
      Entity::id entity{};
    };

    [[nodiscard]] inline Math::Vec2f getPosition(Entity::id)
    {
      return {};
    }
  }
}

//
// Entity::id -> id
//
// id -> Index in Components
//
// Components -> Managed by some functions
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
