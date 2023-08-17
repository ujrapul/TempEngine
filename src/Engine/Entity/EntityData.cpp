// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "EntityData.hpp"
#include "ComponentContainer.hpp"
#include "EngineUtils.hpp"
#include "Entity.hpp"
#include "Logger.hpp"
#include <algorithm>
#include <cassert>
#include <string>

namespace Temp::Entity
{
  namespace
  {
    Entity::id GetAvailable(Data& entityData)
    {
      for (Entity::id i = 0; i < Entity::MAX; ++i)
      {
        if (!entityData.created[i])
        {
          entityData.created[i] = true;
          entityData.used.push_back(i);
          return i;
        }
      }
      return Entity::MAX;
    }

    void ResetAvailable(Data& entityData, Entity::id entity)
    {
      entityData.created[entity] = false;
      entityData.componentBits[entity] = 0;
      entityData.used.erase(std::find(entityData.used.begin(), entityData.used.end(), entity));
    }

#ifdef DEBUG
    void Check(Data& entityData, const std::string& err)
    {
      if (entityData.used.size() >= Entity::MAX)
      {
        Logger::LogErr(err);
      }
    }
#endif
  }

  void Init(Data& entityData)
  {
    entityData = {};
    Component::Container::Init(entityData.componentContainer);
  }

  void Destroy(Data& entityData, Entity::id entity)
  {
#ifdef DEBUG
    Check(entityData, "[Entity] Above limit: " + std::to_string(entity));
#endif
    ResetAvailable(entityData, entity);
    Component::Container::EntityDestroyed(entityData.componentContainer, entity);
#ifdef DEBUG
    Check(entityData, "[Entity] Underflow: " + std::to_string(entity));
#endif
  }

  Entity::id Create(Data& entityData) { return GetAvailable(entityData); }

  ::Temp::ComponentBits& ComponentBits(Data& entityData, Entity::id entity)
  {
#ifdef DEBUG
    Check(entityData, "[Entity] Above limit: " + std::to_string(entity));
#endif
    return entityData.componentBits[entity];
  }

  void Destruct(Data& entityData)
  {
    while (!entityData.used.empty())
    {
      Destroy(entityData, entityData.used.back());
    }
    Component::Container::Destruct(entityData.componentContainer);
  }

  void Reset(Data& entityData)
  {
    Destruct(entityData);
    Init(entityData);
    Component::Container::Reset(entityData.componentContainer);
  }

  size_t Count(Data& entityData) { return entityData.used.size(); }
}
