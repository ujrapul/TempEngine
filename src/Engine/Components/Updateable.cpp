// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Updateable.hpp"
#include "EntityType.hpp"

namespace Temp::Component::Updateable
{
  void Update(Scene::Data& scene, Data& updateable, float deltaTime)
  {
    switch (static_cast<EntityType::EntityType>(updateable.type))
    {
      case EntityType::TEXTBOX:
      case EntityType::TEXTBUTTON:
      case EntityType::SPRITE:
      case EntityType::MAX:
        break;
      case EntityType::MOCK:
        Mock::Update(scene, *static_cast<Mock::Data*>(updateable.data), deltaTime);
        break;
    }
  }
}