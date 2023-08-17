// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "ComponentContainer.hpp"
#include "Entity.hpp"
#include "UT_Common.hpp"

namespace Temp::Component::Container::UnitTests
{
  template <uint8_t T>
  void RunGeneralAsserts(Data& data)
  {
    Assert("Test Component Container Init Equality Test",
           static_cast<Temp::Component::ArrayData<MapToComponentDataType<T>>*>(data.components[T])
               ->array[0] == MapToComponentDataType<T>());
    Assert("Test Component Container Get Array",
           GetComponentArray<T>(data).array[0] == MapToComponentDataType<T>() &&
             GetComponentArray<T>(data).array[Entity::MAX - 1] == MapToComponentDataType<T>());
    Assert("Test Component Container Get Component from Entity",
           Get<T>(data, 15) == MapToComponentDataType<T>() &&
             Get<T>(data, Entity::MAX - 1) == MapToComponentDataType<T>());
    Set<T>(data, 15, GetTestValue<T>());
    AssertEqual("Test Component Container Set Component of Entity::id Equal (1)",
                Get<T>(data, 15),
                GetTestValue<T>());
    AssertNotEqual("Test Component Container Set Component of Entity::id NotEqual (2)",
                   Get<T>(data, 15),
                   MapToComponentDataType<T>());
  }

  template <uint8_t T>
  void Run(Data& data)
  {
    Init<T>(data);
    bool isAllDataInitialized = true;
    auto* arrayData = static_cast<Temp::Component::ArrayData<MapToComponentDataType<T>>*>(
      data.components[T]);
    for (Entity::id e = 0; e < Entity::MAX; ++e)
    {
      isAllDataInitialized &= arrayData->sparseEntities[e] == Entity::MAX;
      isAllDataInitialized &= arrayData->sparseIndices[e] == SIZE_MAX;
    }
    Assert("Test Component Container Init", isAllDataInitialized);
    RunGeneralAsserts<T>(data);
    Destruct<T>(data);
    Assert("Test Component Container Destruct", data.components[T] == nullptr);
  }

  constexpr uint8_t ENUM_MIN = 0;
  constexpr uint8_t ENUM_MAX = Type::HOVERABLE; // Don't want to test new types

  template <uint8_t E>
  void RunEnum(Data& data)
  {
    Run<E>(data);
  }

  template <uint8_t E>
  void InitializeEnum(const Data& data, bool& isInitialized)
  {
    const auto* const arrayData =
      static_cast<Temp::Component::ArrayData<MapToComponentDataType<E>>*>(data.components[E]);
    for (Entity::id e = 0; e < Entity::MAX; ++e)
    {
      isInitialized &= arrayData->sparseEntities[e] == Entity::MAX;
      isInitialized &= arrayData->sparseIndices[e] == SIZE_MAX;
    }
  }

  template <uint8_t E>
  void IsDestructEnum(const Data& data, bool& isDestruct)
  {
    isDestruct &= data.components[E] == nullptr;
  }

  template <uint8_t E>
  struct EnumRange
  {
    static void RunEnums(Data& data)
    {
      RunEnum<E>(data);
      if constexpr (E < ENUM_MAX)
        EnumRange<E + 1>::RunEnums(data);
    }

    static void InitializeEnums(const Data& data, bool& isInitialized)
    {
      InitializeEnum<E>(data, isInitialized);
      if constexpr (E < ENUM_MAX)
        EnumRange<E + 1>::InitializeEnums(data, isInitialized);
    }

    static void IsDestructEnums(const Data& data, bool isDestruct)
    {
      IsDestructEnum<E>(data, isDestruct);
      if constexpr (E < ENUM_MAX)
        EnumRange<E + 1>::IsDestructEnums(data, isDestruct);
    }
  };

  inline void Run()
  {
    Data data;
    EnumRange<ENUM_MIN>::RunEnums(data);

    Init(data);
    bool isAllDataInitialized = true;
    EnumRange<ENUM_MIN>::InitializeEnums(data, isAllDataInitialized);
    Assert("Test Component Container Init All", isAllDataInitialized);
    Destruct(data);

    Init(data);
    Set<0>(data, 15, GetTestValue<0>());
    Set<3>(data, 15, GetTestValue<3>());
    EntityDestroyed(data, 15);
    Assert("Test Component Container Entity::id Destroyed",
           Get<0>(data, 15) == MapToComponentDataType<0>() &&
             Get<3>(data, 15) == MapToComponentDataType<3>());

    Destruct(data);
    bool isAllDataDestructed = true;
    EnumRange<ENUM_MIN>::IsDestructEnums(data, isAllDataDestructed);
    Assert("Test Component Container Destruct All", isAllDataDestructed);
  }
}
