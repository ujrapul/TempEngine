// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "ComponentType.hpp"
#include "Null.hpp"

namespace Temp
{
  namespace Component
  {
    namespace GameType
    {
      enum GameType
      {
        TEMP = Temp::Component::Type::MAX,
        MAX
      };
    }

    using namespace Temp::Component::GameType;

    template <> struct MapToComponentDataType_t<TEMP> { using type = NullObject::Data; };
  }
}
