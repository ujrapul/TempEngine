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

#pragma once

#include "Entity.hpp"
#include "Components/Component.hpp"
#include <queue>
#include <array>

namespace Temp
{
  namespace EntityManager
  {
    using EntityQueue = std::queue<Entity>;
    using SignatureArray = std::array<Signature, MAX_ENTITIES>;

    struct Data
    {
      EntityQueue availableEntities{};
      SignatureArray signatures{};
      std::vector<Entity> currentEntities{};
      uint32_t livingEntityCount{};
    };
    
    void InitData(Data& entityManager);
    [[nodiscard]] Entity CreateEntity(Data& entityManager);
    void DestroyEntity(Data& entityManager, Entity entity);
    void SetSignature(Data& entityManager, Entity entity, Signature signature);
    [[nodiscard]] const Signature& GetSignature(const Data& entityManager, Entity entity);
    void Destruct(Data& entityManager);
    void Reset(Data& entityManager);
  }
}
