// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "ComponentType.hpp"
#include "EngineUtils.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "Null.hpp"
#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>

namespace Temp::Scene
{
  struct Data;
}

namespace Temp::SceneObject
{
  namespace FnType
  {
    enum FnType
    {
      CONSTRUCT = 0,
      DRAWCONSTRUCT,
      DRAWDESTRUCT,
      DESTRUCT,
      MAX
    };
  }

  struct Data
  {
    void* data{nullptr};
    void* constructData{nullptr};
    std::string name{""};
    int type{EntityType::MAX};
    int shaderType{-1};
    Entity::id entity{Entity::MAX};

    // Needed for unit test
    bool operator==(const Data& other) const = default;
  };

#ifdef EDITOR
  template <uint8_t T>
  inline Data Copy(const Data& object)
  {
    Data copy{object};
    copy.data = new EntityType::MapToType<T>(*static_cast<EntityType::MapToType<T>*>(object.data));
    copy.constructData = new EntityType::MapToCtorType<T>(
      *static_cast<EntityType::MapToCtorType<T>*>(object.constructData));
    return copy;
  }
#endif

  // NOTE: For now make sure to only use this in DrawConstruct or DrawUpdate
  void SetActive(Scene::Data& scene, Data& object, bool active);

  inline std::ostream& operator<<(std::ostream& os, const Data& object)
  {
    os << "SceneObject(" << object.data << " " << object.constructData << " " << object.name << " "
       << object.type << " " << object.entity << ")";
    return os;
  }

  inline void (*FnTable[FnType::MAX][256])(Scene::Data&, Data&);
#ifdef EDITOR
  inline Data (*CopyTable[256])(Data&);
#endif

  constexpr void Construct(Scene::Data& scene, Data& object)
  {
    FnTable[FnType::CONSTRUCT][object.type](scene, object);
  }

  constexpr void Destruct(Scene::Data& scene, Data& object)
  {
    FnTable[FnType::DESTRUCT][object.type](scene, object);
  }

  constexpr void DrawConstruct(Scene::Data& scene, Data& object)
  {
    FnTable[FnType::DRAWCONSTRUCT][object.type](scene, object);
  }

  constexpr void DrawDestruct(Scene::Data& scene, Data& object)
  {
    FnTable[FnType::DRAWDESTRUCT][object.type](scene, object);
  }

#ifdef EDITOR
  inline Data Copy(Data& object) { return CopyTable[object.type](object); }
#endif

  void Init();
  void ClearActiveDataCache();

  void Translate(Scene::Data& scene, const SceneObject::Data& object, const Math::Vec3f& position);
  void Translate(Scene::Data& scene, const SceneObject::Data& object, const Math::Vec2f& position);
  void Scale(Scene::Data& scene, const SceneObject::Data& object, float scale);
  void Scale(Scene::Data& scene, const SceneObject::Data& object, const Math::Vec2f& scale);
};
