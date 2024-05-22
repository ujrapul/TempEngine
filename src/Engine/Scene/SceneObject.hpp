// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array_fwd.hpp"
#include "ComponentType.hpp"
#include "EngineUtils.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "HashMap.hpp"
#include "MemoryManager.hpp"
#include "Null.hpp"
#include "String.hpp"

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
    SceneString name{};
    SceneDynamicArray<SceneString> properties{};
    int type{EntityType::MAX};
    int shaderType{-1};
    Entity::id entity{Entity::MAX};
    MemoryManager::Data::Type allocationType{MemoryManager::Data::Type::SCENE_ARENA};

    // Needed for unit test
    bool operator==(const Data& other) const = default;
  };

#ifdef EDITOR
  template <uint8_t T>
  inline Data Copy(const Data& object)
  {
    Data copy{object};
    copy.data = MemoryManager::CreateScene<EntityType::MapToType<T>>(
      *static_cast<EntityType::MapToType<T>*>(object.data));
    copy.constructData = MemoryManager::CreateScene<EntityType::MapToCtorType<T>>(
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

  inline void Construct(Scene::Data& scene, Data& object)
  {
    FnTable[FnType::CONSTRUCT][object.type](scene, object);
  }

  inline void Destruct(Scene::Data& scene, Data& object)
  {
    FnTable[FnType::DESTRUCT][object.type](scene, object);
  }

  inline void DrawConstruct(Scene::Data& scene, Data& object)
  {
    FnTable[FnType::DRAWCONSTRUCT][object.type](scene, object);
  }

  inline void DrawDestruct(Scene::Data& scene, Data& object)
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
