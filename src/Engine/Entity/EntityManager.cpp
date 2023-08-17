// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

//  Built with reference from Austin Morlan's ECS
//
//  Original work Copyright © 2020 Austin Morlan. All rights reserved.
//  Modified work Copyright © 2023 Ujwal Vujjini. All rights reserved.
//
//  MIT License
//
//  Copyright (c) 2020 Austin Morlan
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice (including the next paragraph)
//  shall be included in all copies or substantial portions of the
//  Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
//  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
//  OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//  https://austinmorlan.com/posts/entity_component_system/
//  https://code.austinmorlan.com/austin/2019-ecs

#include "EntityManager.hpp"
#include "Entity.hpp"
#include "EngineUtils.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace Temp::EntityManager
{
  void InitData(Data &entityManager)
  {
    entityManager.currentEntities.clear();
    entityManager.livingEntityCount = 0;
    entityManager.signatures.fill({});
    // Needed to make sure we're not adding more than MAX_ENTITIES
    // There was a bug where without the swap we continously added
    // MAX_ENTITIES elements into the queue every reset causing the
    // memory usage to substantially increase every iteration
    FreeContainer(entityManager.availableEntities);
    for (Entity e = 0; e < MAX_ENTITIES; ++e)
    {
      entityManager.availableEntities.push(e);
    }
  }

  Entity CreateEntity(Data &entityManager)
  {
    assert(entityManager.livingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

    Entity id = entityManager.availableEntities.front();
    entityManager.availableEntities.pop();
    ++entityManager.livingEntityCount;

    entityManager.currentEntities.push_back(id);

    return id;
  }

  void DestroyEntity(Data &entityManager, Entity entity)
  {
    assert(entity < MAX_ENTITIES && "Entity out of range.");

    // Invalidate the destroyed entity's signature
    entityManager.signatures[entity].reset();

    // Put the destroyed ID at the back of the queue
    entityManager.availableEntities.push(entity);
    --entityManager.livingEntityCount;

    entityManager.currentEntities.erase(std::find(entityManager.currentEntities.begin(), entityManager.currentEntities.end(), entity));

    assert(entityManager.livingEntityCount < MAX_ENTITIES && "Living Entity Count underflowed!");
  }

  void SetSignature(Data &entityManager, Entity entity, Signature signature)
  {
    assert(entity < MAX_ENTITIES && "Entity out of range.");

    // Put this entity's signature into the array
    entityManager.signatures[entity] = signature;
  }

  const Signature& GetSignature(const Data &entityManager, Entity entity)
  {
    assert(entity < MAX_ENTITIES && "Entity out of range.");

    // Get this entity's signature from the array
    return entityManager.signatures[entity];
  }

  void Destruct(Data &entityManager)
  {
    while (!entityManager.currentEntities.empty())
    {
      DestroyEntity(entityManager, entityManager.currentEntities.back());
    }
  }

  void Reset(Data &entityManager)
  {
    Destruct(entityManager);
    InitData(entityManager);
  }
}
