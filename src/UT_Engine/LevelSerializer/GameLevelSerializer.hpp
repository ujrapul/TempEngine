// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "LevelSerializer.hpp"
#include "GameEntityType.hpp"

namespace Temp::LevelSerializer
{
  inline bool ExtensionDeserializer(LevelSerializer::GlobalDeserializeData &)
  {
    return true;
  }

  inline void ExtensionSerializer(LevelSerializer::GlobalSerializeData &, const SceneObject::Data &)
  {

  }
}
