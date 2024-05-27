// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "EntityData.hpp"
#include "ComponentContainer.hpp"
#include "Entity.hpp"
#include "Logger.hpp"

namespace Temp::Entity
{
  namespace
  {
    Entity::id numEntities = 0;

    Entity::id GetAvailable(Data& entityData)
    {
      for (Entity::id i = 0; i < Entity::MAX; ++i)
      {
        if (!entityData.used[i])
        {
          entityData.used[i] = true;
          ++numEntities;
          return i;
        }
      }
      return Entity::MAX;
    }

    void ResetAvailable(Data& entityData, Entity::id entity)
    {
      entityData.componentBits[entity] = 0;
      entityData.used[entity] = false;
      --numEntities;
    }

#ifdef DEBUG
    void Check(const String& err)
    {
      if (numEntities >= Entity::MAX)
      {
        Logger::LogErr(err);
      }
    }
#endif
  }

  void Init(Data& entityData)
  {
    entityData = {};
    numEntities = 0;
    entityData.used.Reserve(Entity::MAX);
    entityData.componentBits.Reserve(Entity::MAX);
    Component::Container::Init(entityData.componentContainer);
  }

  void Destroy(Data& entityData, Entity::id entity)
  {
#ifdef DEBUG
    Check(String("[Entity] Above limit: ") + String::ToString(entity));
#endif
    ResetAvailable(entityData, entity);
    Component::Container::EntityDestroyed(entityData.componentContainer, entity);
#ifdef DEBUG
    Check(String("[Entity] Underflow: ") + String::ToString(entity));
#endif
  }

  Entity::id Create(Data& entityData) { return GetAvailable(entityData); }

  ::Temp::ComponentBits& ComponentBits(Data& entityData, Entity::id entity)
  {
#ifdef DEBUG
    Check(String("[Entity] Above limit: ") + String::ToString(entity));
#endif
    return entityData.componentBits[entity];
  }

  void Destruct(Data& entityData)
  {
    for (size_t i = 0; i < entityData.used.size; ++i)
    {
      if (entityData.used[i])
      {
        Destroy(entityData, i);
        --numEntities;
      }
    }
    Component::Container::Destruct(entityData.componentContainer);
  }

  void Reset(Data& entityData)
  {
    Destruct(entityData);
    Init(entityData);
    Component::Container::Reset(entityData.componentContainer);
  }

  size_t Count(Data& entityData) { return numEntities; }
}
