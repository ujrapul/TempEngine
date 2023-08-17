// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Drawable.hpp"
#include "EntityData.hpp"
#include "SceneObject.hpp"
#include "ThreadPool.hpp"
#include "lua.h"
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace Temp::SceneObject
{
  struct Data;
}

namespace Temp::LevelSerializer
{
  struct GlobalSerializeData;
  struct GlobalDeserializeData;
}

struct lua_State;

namespace Temp::Scene
{
  enum class State : uint8_t
  {
    ENTER = 0,
    RUN = 1,
    LEAVE = 2,
    MAX = 3
  };

  inline void NoOpScene(struct Data&) {}

  void Construct(Data& scene);
  void Update(Data& scene, float deltaTime);
  // Only here for Performance Testing!
  void UpdateLegacy(Data& scene, lua_State* L, float deltaTime);
  void Destruct(Data& scene);
  void DrawConstruct(Data& scene);
  void DrawDestruct(Data& scene);
  void DrawUpdate(Data& scene);
  void DrawReload(Data& scene, int shaderIdx);

  void Draw(Data& scene);

  typedef void (*RenderFunction)(Data&, void*);

  struct RenderData
  {
    RenderFunction func;
    void* data;
  };

  struct SceneFns
  {
    void (*ConstructFunc)(Scene::Data&){NoOpScene};
    void (*UpdateFunc)(Scene::Data&, float){[](auto&, auto) {}};
    void (*DestructFunc)(Scene::Data&){NoOpScene};
    void (*DrawConstructFunc)(Scene::Data&){NoOpScene};
    void (*DrawDestructFunc)(Scene::Data&){NoOpScene};
    void (*DrawUpdateFunc)(Scene::Data&){NoOpScene};
    SceneFns* nextScene{nullptr};
    // Should refer to the name of the .level file
    std::string name{};
  };

  struct Data
  {
    std::vector<SceneObject::Data> objects{};
    std::unordered_map<std::string, int> objectsNameIdxTable{};
    std::unordered_map<Entity::id, int> entityObjectIdxTable{};
    std::queue<RenderData> renderQueue{};
    Entity::Data entityData{};
    State state{State::ENTER};
    // Threadpool should be recreated when assigned or copied
    ThreadPool::Data threadPool{};
    SceneFns sceneFns{};

    Data& operator=(const Data& other)
    {
      objects = other.objects;
      objectsNameIdxTable = other.objectsNameIdxTable;
      renderQueue = other.renderQueue;
      entityData = other.entityData;
      state = other.state;
      sceneFns = other.sceneFns;
      return *this;
    }
  };

  void Initialize(Data& scene);
  void Destroy(Data& scene);

  Entity::id CreateEntity(Data& scene);
  void DestroyEntity(Data& scene, Entity::id entity);
  void EnqueueRender(Scene::Data& scene, RenderFunction func, void* data);
  void ClearRender(Scene::Data& scene);
  SceneObject::Data& GetObject(Scene::Data& scene, const std::string& name);
  const SceneObject::Data& GetObject(const Scene::Data& scene, const std::string& name);
  SceneObject::Data& GetObject(Scene::Data& scene, Entity::id entity);
  const SceneObject::Data& GetObject(const Scene::Data& scene, Entity::id entity);
  // Use this if spawning in the middle of game session
  void SpawnObject(Scene::Data& scene, const SceneObject::Data& object);
  int AddObject(Scene::Data& scene, const SceneObject::Data& object);
  void RemoveObject(Scene::Data& scene, const SceneObject::Data& object);
  bool ValidateObjectName(const Scene::Data& scene, const std::string& name);
  void UpdateObjectName(Scene::Data& scene, SceneObject::Data& object, const std::string& name);

  template <uint8_t T>
  constexpr void AddComponent(Data& scene,
                              Entity::id entity,
                              const Component::MapToComponentDataType<T>& component)
  {
    Entity::AddComponent<T>(scene.entityData, entity, component);
  }

  template <uint8_t T>
  constexpr void RemoveComponent(Data& scene, Entity::id entity)
  {
    Entity::RemoveComponent<T>(scene.entityData, entity);
  }

  template <uint8_t T>
  [[nodiscard]] constexpr Component::MapToComponentDataType<T>& Get(Data& scene, Entity::id entity)
  {
    return Entity::Get<T>(scene.entityData, entity);
  }

  template <uint8_t T>
  [[nodiscard]] constexpr Component::ArrayData<Component::MapToComponentDataType<T>>&
  GetComponentArray(Data& scene)
  {
    return Entity::GetComponentArray<T>(scene.entityData);
  }

  [[nodiscard]] inline ComponentBits& ComponentBits(Data& scene, Entity::id entity)
  {
    return Entity::ComponentBits(scene.entityData, entity);
  }
}
