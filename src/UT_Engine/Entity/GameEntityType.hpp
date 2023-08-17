// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "EntityType.hpp"

namespace Temp::GameEntityType
{
  enum GameEntityType
  {
    TEMP = EntityType::MAX,
    MAX
  };
}

namespace Temp::EntityType
{
  template <> struct MapToType_t<GameEntityType::TEMP> { using type = NullObject::Data; };
  template <> struct MapToCtorType_t<GameEntityType::TEMP> { using type = Null; };
}