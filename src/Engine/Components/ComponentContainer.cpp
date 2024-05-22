// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "ComponentContainer.hpp"

#include "Entity.hpp"
#include "GameComponentType.hpp"

namespace Temp::Component::Container
{
  namespace
  {
    constexpr uint8_t ENUM_MIN = 0;
    constexpr uint8_t ENUM_MAX = Temp::Component::GameType::MAX - 1;

    template <uint8_t E>
    void InitEnum(Data& data)
    {
      if constexpr (std::is_same<MapToComponentDataType<E>, Null>::value)
      {
        return;
      }
      Init<E>(data);
    }

    template <uint8_t E>
    void DestructEnum(Data& data)
    {
      if constexpr (std::is_same<MapToComponentDataType<E>, Null>::value)
      {
        return;
      }
      Destruct<E>(data);
    }

    template <uint8_t E>
    void EntityDestroyedEnum(Data& data, Entity::id entity)
    {
      if constexpr (std::is_same<MapToComponentDataType<E>, Null>::value)
      {
        return;
      }

      Component::EntityDestroyed(GetComponentArray<E>(data), entity);
    }

    template <uint8_t E>
    void ResetEnum(Data& data)
    {
      if constexpr (std::is_same<MapToComponentDataType<E>, Null>::value)
      {
        return;
      }
      Reset<E>(data);
    }

    template <uint8_t E, uint8_t ENUM_MAX>
    struct EnumRange
    {
      static void InitEnums(Data& data)
      {
        InitEnum<E>(data);
        if constexpr (E < ENUM_MAX)
          EnumRange<E + 1, ENUM_MAX>::InitEnums(data);
      }

      static void DestructEnums(Data& data)
      {
        DestructEnum<E>(data);
        if constexpr (E < ENUM_MAX)
          EnumRange<E + 1, ENUM_MAX>::DestructEnums(data);
      }

      static void EntityDestroyedEnums(Data& data, Entity::id entity)
      {
        EntityDestroyedEnum<E>(data, entity);
        if constexpr (E < ENUM_MAX)
          EnumRange<E + 1, ENUM_MAX>::EntityDestroyedEnums(data, entity);
      }

      static void ResetEnums(Data& data)
      {
        ResetEnum<E>(data);
        if constexpr (E < ENUM_MAX)
          EnumRange<E + 1, ENUM_MAX>::ResetEnums(data);
      }
    };
  }

  void Init(Data& data) { EnumRange<ENUM_MIN, ENUM_MAX>::InitEnums(data); }

  void Destruct(Data& data) { EnumRange<ENUM_MIN, ENUM_MAX>::DestructEnums(data); }

  void EntityDestroyed(Data& data, Entity::id entity)
  {
    EnumRange<ENUM_MIN, ENUM_MAX>::EntityDestroyedEnums(data, entity);
  }

  void Reset(Data& data) { EnumRange<ENUM_MIN, ENUM_MAX>::ResetEnums(data); }
}

namespace Temp
{
  template struct Array<Component::Null, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  template struct Array<Entity::id, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  template struct Array<std::size_t, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  template struct Array<Component::CacheData<Component::Null>, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;

  template struct Array<Math::Vec2f, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  template struct Array<Component::CacheData<Math::Vec2f>, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;

  template struct Array<Component::Drawable::Data, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  template struct Array<Component::CacheData<Component::Drawable::Data>, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;

  template struct Array<SceneString, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  template struct Array<Component::CacheData<SceneString>, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;

  template struct Array<Component::Hoverable::Data, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  template struct Array<Component::CacheData<Component::Hoverable::Data>, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;

  template struct Array<Component::Updateable::Data, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;
  template struct Array<Component::CacheData<Component::Updateable::Data>, Entity::MAX, MemoryManager::Data::Type::SCENE_ARENA>;

  template Temp::Math::Vec2<float> Temp::Component::dummy<Temp::Math::Vec2<float>>;
  template Temp::BaseString<Temp::MemoryManager::Data::Type::SCENE_ARENA> Temp::Component::dummy<Temp::BaseString<Temp::MemoryManager::Data::Type::SCENE_ARENA>>;
  template Temp::Component::Drawable::Data Temp::Component::dummy<Temp::Component::Drawable::Data>;
  template Temp::Component::Hoverable::Data Temp::Component::dummy<Temp::Component::Hoverable::Data>;
}