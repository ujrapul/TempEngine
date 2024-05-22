// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array_fwd.hpp"
#include "Entity.hpp"
#include "EntityData.hpp"
#include "SceneObject.hpp"
#include "String.hpp"
#include "ThreadPool.hpp"
#include "HashMap.hpp"
#include <utility>

namespace Temp::SceneObject
{
  struct Data;
}

namespace Temp::LevelSerializer
{
  struct GlobalSerializeData;
  struct GlobalDeserializeData;
}

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
  void UpdateLegacy(Data& scene, float deltaTime);
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
    GlobalString name{};

    constexpr bool operator==(const SceneFns& other) const = default;
  };

  struct Data
  {
    SceneDynamicArray<SceneObject::Data> objects{};
    SceneStringHashMap<int> objectsNameIdxTable{};
    SceneDynamicArray<int> entityObjectIdxTable{};
    std::queue<RenderData> renderQueue{};
    Entity::Data entityData{};
    State state{State::ENTER};
    // Threadpool should be recreated when assigned or copied
    ThreadPool::Data threadPool{};
    SceneFns* sceneFns{nullptr};
    void* gameData{nullptr};

    Data(){}

    Data(Data& other)
      : objects(other.objects),
        objectsNameIdxTable(other.objectsNameIdxTable),
        entityObjectIdxTable(other.entityObjectIdxTable),
        renderQueue(other.renderQueue),
        entityData(other.entityData),
        state(other.state),
        sceneFns(other.sceneFns),
        gameData(other.gameData)
    {
    }

    Data(Data&& other)
      : Data()
    {
      Swap(*this, other);
    }

    Data& operator=(Data other)
    {
      Swap(*this, other);
      return *this;
    }

    constexpr void Swap(Data& first, Data& second)
    {
      Utils::Swap(first.objects, second.objects);
      Utils::Swap(first.objectsNameIdxTable, second.objectsNameIdxTable);
      Utils::Swap(first.entityObjectIdxTable, second.entityObjectIdxTable);
      Utils::Swap(first.renderQueue, second.renderQueue);
      Utils::Swap(first.entityData, second.entityData);
      Utils::Swap(first.state, second.state);
      Utils::Swap(first.sceneFns, second.sceneFns);
      Utils::Swap(first.gameData, second.gameData);
    }
  };

  void Initialize(Data& scene);
  void Destroy(Data& scene);

  Entity::id CreateEntity(Data& scene);
  void DestroyEntity(Data& scene, Entity::id entity);
  void EnqueueRender(Scene::Data& scene, RenderFunction func, void* data);
  void ClearRender(Scene::Data& scene);
  SceneObject::Data& GetObject(Scene::Data& scene, const char* name);
  const SceneObject::Data& GetObject(const Scene::Data& scene, const char* name);
  SceneObject::Data& GetObject(Scene::Data& scene, Entity::id entity);
  const SceneObject::Data& GetObject(const Scene::Data& scene, Entity::id entity);
  // Use this if spawning in the middle of game session
  void SpawnObject(Scene::Data& scene, const SceneObject::Data& object);
  int AddObject(Scene::Data& scene, const SceneObject::Data& object);
  void RemoveObject(Scene::Data& scene, Entity::id entity);
  bool ValidateObjectName(const Scene::Data& scene, const char* name);
  void UpdateObjectName(Scene::Data& scene, SceneObject::Data& object, const char* name);

  template <uint8_t T>
  constexpr void AddComponent(Data& scene,
                              Entity::id entity,
                              Component::MapToComponentDataType<T> component)
  {
    Entity::AddComponent<T>(scene.entityData, entity, std::move(component));
  }

  template <uint8_t T>
  constexpr void AddCacheComponent(Data& scene, Entity::id entity)
  {
    Entity::AddCacheComponent<T>(scene.entityData, entity);
  }

  template <uint8_t T>
  constexpr void RemoveCacheComponent(Data& scene, Entity::id entity)
  {
    Entity::RemoveCacheComponent<T>(scene.entityData, entity);
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

  void LoadSceneFnsFromDynamicLibrary(const std::string& name, SceneFns& sceneFns);
}

extern template std::_Deque_base<Temp::Scene::RenderData, std::allocator<Temp::Scene::RenderData> >::_Deque_base::~_Deque_base();
