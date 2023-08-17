// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "ComponentData.hpp"
#include "Entity.hpp"
#include "UT_Common.hpp"

namespace Temp::Component::UnitTests
{
  template <typename T>
  inline void TestComponents(ArrayData<T>& data,
                             T Component1,
                             T Component2,
                             T Component3,
                             T Component4,
                             T Component5,
                             T Component6)
  {
    Init(data);
    bool isAllDataInitialized = true;
    for (Entity::id e = 0; e < Entity::MAX; ++e)
    {
      isAllDataInitialized &= data.sparseEntities[e] == Entity::MAX;
      isAllDataInitialized &= data.sparseIndices[e] == SIZE_MAX;
    }
    Assert("Test ArrayData Init", isAllDataInitialized);

    Set(data, 5, Component1);
    Set(data, 15, Component2);
    Set(data, 3, Component3);
    Set(data, 8, Component4);
    Assert("Test ArrayData Set Value Array",
           data.array[0] == Component1 && data.array[1] == Component2 &&
             data.array[2] == Component3 && data.array[3] == Component4);
    Assert("Test ArrayData Set Value IndexToEntity",
           data.sparseEntities[0] == 5 && data.sparseEntities[1] == 15 &&
             data.sparseEntities[2] == 3 && data.sparseEntities[3] == 8);
    Assert("Test ArrayData Set Value EntityToIndex",
           data.sparseIndices[5] == 0 && data.sparseIndices[15] == 1 &&
             data.sparseIndices[3] == 2 && data.sparseIndices[8] == 3);
    Assert("Test ArrayData Set Value Size", data.size == 4);
    Set(data, 15, Component5);
    Assert("Test ArrayData Set Value to Same Entity::id Array", data.array[1] == Component5);
    Assert("Test ArrayData Set Value to Same Entity::id Size", data.size == 4);
    Remove(data, 15);
    Assert("Test ArrayData Remove Value Array",
           data.array[0] == Component1 && data.array[1] == Component4 &&
             data.array[2] == Component3 && data.array[3] == T());
    Assert("Test ArrayData Remove Value Size", data.size == 3);
    auto test = Get(data, 3);
    auto test2 = Get(data, 15);
    auto test3 = Get(data, 5);
    auto test4 = T();
    Assert(
      "Test ArrayData Get Value",
      Get(data, 8) == Component4 && test2 == test4 && test == Component3 && test3 == Component1);
    Get(data, 3) = Component6;
    Assert("Test ArrayData Get Value and Modify", data.array[data.sparseIndices[3]] == Component6);
    EntityDestroyed(data, 3);
    Assert("Test ArrayData Entity::id Destroyed Size", data.size == 2);
    Assert("Test ArrayData Entity::id Destroyed Array",
           data.array[0] == Component1 && data.array[1] == Component4 && data.array[2] == T() &&
             data.array[3] == T());
  }

  inline void Run()
  {
    // Simple Component Test
    {
      ArrayData<int> intData{};
      TestComponents(intData, 73234, 2342834, -2382, 999999, 370846540, 2383);
    }

    // Complex Component Test
    {
      ArrayData<MockComponent> mockData{};
      TestComponents(mockData,
                     {"q", 234, false},
                     {"fsa", 478575, false},
                     {"epjirovagnerj", -10234, true},
                     {"dblndfb/.324-", 0, true},
                     {"smcos ", 349654, false},
                     {"//.,;'[]-=", 3, true});
    }
  }
}
