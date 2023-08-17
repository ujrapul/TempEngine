#pragma once

#include "ComponentType.hpp"
#include "Null.hpp"
#include <vector>

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
