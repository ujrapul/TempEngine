// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Scene.hpp"
#include "Array_fwd.hpp"
#include "ComponentType.hpp"
#include "Drawable.hpp"
#include "EditorGrid.hpp"
#include "Engine.hpp"
#include "EngineUtils.hpp"
#include "Entity.hpp"
#include "EntityData.hpp"
#include "HashMap.hpp"
#include "Hoverable.hpp"
#ifndef EDITOR
#include "LevelSerializer.hpp"
#endif
#include "Logger.hpp"
#include "MemoryManager.hpp"
#include "SceneObject.hpp"
#include "Shader.hpp"
#include "TextBox.hpp"
#include "ThreadPool.hpp"
#include "Updateable.hpp"

namespace Temp::Scene
{
  namespace
  {
    ThreadPool::Data threadPool{};

#ifdef EDITOR
    EditorGrid::Data editorGrid;
#endif
    SceneObject::Data dummy;

#ifndef EDITOR
    bool isDeserialized = false;
#endif

    void DequeueRender(Scene::Data& scene)
    {
      while (!scene.renderQueue.Empty())
      {
        auto& render = scene.renderQueue.Front();
        render.func(scene, render.data);
        scene.renderQueue.Pop();
      }
    }

    void CreateEntity(Scene::Data& scene, int index)
    {
      Entity::id entity = Scene::CreateEntity(scene);
      scene.entityObjectIdxTable[entity] = index;
      scene.objects[index].entity = entity;
    }

    void DestroyEntity(Scene::Data& scene, SceneObject::Data& object)
    {
      Temp::Scene::DestroyEntity(scene, object.entity);
      scene.objectsNameIdxTable[object.name.c_str()] = INT_MAX;
      scene.entityObjectIdxTable[object.entity] = INT_MAX;
    }
  }

  void Construct(Data& scene, bool resetData)
  {
    if (resetData)
    {
      Scene::ResetAllocatedTypes(scene);
    }
#if not defined(UT)
#if not defined(EDITOR)
    isDeserialized = LevelSerializer::Deserialize(scene, (String(scene.sceneFns->name.c_str()) + ".level").c_str());
    if (isDeserialized)
    {
#else
    EditorGrid::Construct(scene, editorGrid);
#endif
      for (size_t i = 0; i < scene.objects.size; ++i)
      {
        CreateEntity(scene, (int)i);
        SceneObject::Construct(scene, scene.objects[i]);
      }
#ifndef EDITOR
      scene.sceneFns->ConstructFunc(scene);
    }
#endif
#endif
  }

  void Destruct(Data& scene)
  {
    for (auto& object : scene.objects)
    {
      SceneObject::Destruct(scene, object);
      DestroyEntity(scene, object);
    }
#ifdef EDITOR
    EditorGrid::Destruct(scene, editorGrid);
#endif
    scene.objectsNameIdxTable.Clear();
    scene.entityObjectIdxTable.Clear();
    scene.sceneFns->DestructFunc(scene);
    SceneObject::ClearActiveDataCache();
    MemoryManager::data.FreeAll();
    dummy = {};
    if (Global::IsActive())
    {
      Entity::Reset(scene.entityData);
    }
    else
    {
      Entity::Destruct(scene.entityData);
    }
  }

  void UpdateLegacy(Data& scene, float deltaTime)
  {
    auto& updateableArray = Scene::GetComponentArray<Component::Type::UPDATEABLE>(scene);
    for (size_t i = 0; i < updateableArray.size; ++i)
    {
      Component::Updateable::Update(scene, updateableArray.array[i], deltaTime);
    }
    scene.sceneFns->UpdateFunc(scene, deltaTime);
  }

  void Update(Data& scene, float deltaTime)
  {
    {
      // DONT SCOPE MEMORY HERE | IT WILL CLASH WITH OTHER THREADS
      // auto f = [&scene, deltaTime](Component::Updateable::Data& updateable) {
      //   Component::Updateable::Update(scene, updateable, deltaTime);
      // };
      auto& updateableArray = Scene::GetComponentArray<Component::Type::UPDATEABLE>(scene);

      struct TaskData
      {
        Component::Updateable::Data* updateable{nullptr};
        Scene::Data* scene{nullptr};
        float deltaTime;
      };
      DynamicArray<void*> tasks;
      for (size_t i = 0; i < updateableArray.size; ++i)
      {
        tasks.PushBack(MemoryManager::CreateTemp<TaskData>(&updateableArray.array[i], &scene, deltaTime));
      }
      auto f = [](void* data) {
        TaskData* taskData = static_cast<TaskData*>(data);
        Component::Updateable::Update(*taskData->scene, *taskData->updateable, taskData->deltaTime);
      };
      ThreadPool::EnqueueForEach(threadPool, tasks.buffer, f, tasks.size);
      ThreadPool::Wait(threadPool);
    }

    scene.sceneFns->UpdateFunc(scene, deltaTime);
  }

  void DrawConstruct(Data& scene)
  {
#ifdef EDITOR
    EditorGrid::DrawConstruct(scene, editorGrid);
#endif
    for (auto& object : scene.objects)
    {
      SceneObject::DrawConstruct(scene, object);
    }

    scene.sceneFns->DrawConstructFunc(scene);
  }

  void DrawDestruct(Data& scene)
  {
#ifdef EDITOR
    EditorGrid::DrawDestruct(scene, editorGrid);
#endif
    for (auto& object : scene.objects)
    {
      SceneObject::DrawDestruct(scene, object);
    }
    auto& drawableArray = Scene::GetComponentArray<Component::Type::DRAWABLE>(scene);
    for (size_t i = 0; i < drawableArray.size; ++i)
    {
      Component::Drawable::Destruct(drawableArray.array[i]);
    }

    scene.sceneFns->DrawDestructFunc(scene);
  }

  void DrawUpdate(Data& scene) { scene.sceneFns->DrawUpdateFunc(scene); }

#ifdef DEBUG
  void DrawReload(Data& scene, int shaderIdx)
#else
  void DrawReload(Data& scene, int)
#endif
  {
#ifndef EDITOR
    if (!isDeserialized)
    {
      return;
    }
#else
    if (shaderIdx == Render::EditorShaderIdx::EDITORGRID)
    {
      Temp::EditorGrid::DrawDestruct(scene, editorGrid);
      Temp::EditorGrid::DrawConstruct(scene, editorGrid);
      return;
    }
#endif

    auto& drawableArray = Scene::GetComponentArray<Component::Type::DRAWABLE>(scene);

    for (size_t i = 0; i < drawableArray.size; ++i)
    {
      auto& drawable = drawableArray.array[i];
#ifdef DEBUG
      if (shaderIdx != drawable.shaderIdx)
      {
        continue;
      }
#endif

      auto& object = scene.objects[scene.entityObjectIdxTable[drawable.entity]];
      SceneObject::DrawDestruct(scene, object);
      SceneObject::DrawConstruct(scene, object);
    }
  }

  void Initialize(Data& scene)
  {
    Entity::Init(scene.entityData);
    ThreadPool::Initialize(threadPool);
  }

  void Destroy(Data& scene)
  {
    Entity::Destruct(scene.entityData);
    ThreadPool::Destruct(threadPool);
  }

  void ResetAllocatedTypes(Data& scene)
  {
    scene.objects = SceneDynamicArray<SceneObject::Data>(true, Entity::MAX);
    scene.objectsNameIdxTable = SceneStringHashMap<int, Entity::MAX>();
    scene.entityObjectIdxTable = SceneDynamicArray<int>(true, Entity::MAX);
    scene.renderQueue = SceneQueue<RenderData>();
  }

  Entity::id CreateEntity(Data& scene) { return Entity::Create(scene.entityData); }

  void DestroyEntity(Data& scene, Entity::id entity) { Entity::Destroy(scene.entityData, entity); }

  void EnqueueRender(Data& scene, RenderFunction func, void* data)
  {
    scene.renderQueue.Push({func, data});
  }

  void Draw(Data& scene)
  {
    // NOTE: Below true only for multi-threaded render code
    // Avoid using the Render Queue for real-time updates to avoid flickering!
    DequeueRender(scene);
    auto& drawableArray = Scene::GetComponentArray<Component::Type::DRAWABLE>(scene);
    for (size_t i = 0; i < drawableArray.size; ++i)
    {
#ifdef EDITOR
      if (drawableArray.array[i].entity == editorGrid.entity)
      {
        // A hack to get the editor grid to render
        static SceneObject::Data obj;
        Component::Drawable::Draw(
          scene,
          obj,
          drawableArray.array[i]);
        continue;
      }
#endif
      Component::Drawable::Draw(
        scene,
        scene.objects[scene.entityObjectIdxTable[drawableArray.array[i].entity]],
        drawableArray.array[i]);
      // Component::Drawable::DrawUpdate(
      //   scene,
      //   scene.objects[scene.entityObjectIdxTable[drawableArray.array[i].entity]],
      //   drawableArray.array[i]);
    }
#ifdef EDITOR
    auto& hoverableArray = Scene::GetComponentArray<Component::Type::HOVERABLE>(scene);
    for (size_t i = 0; i < hoverableArray.size; ++i)
    {
      Component::Drawable::Draw(
        scene,
        scene.objects[scene.entityObjectIdxTable[hoverableArray.array[i].drawable.entity]],
        hoverableArray.array[i].drawable, GL_LINE);
    }
#endif
    Temp::Camera::UpdateOrthoScale(scene, (720.f / Temp::Camera::GetHeight()));
    Temp::Camera::UpdateFontOrthoScale(scene, (720.f / Temp::Camera::GetHeight()));
  }

  void ClearRender(Data& scene) { scene.renderQueue.Clear(); }

  SceneObject::Data& GetObject(Scene::Data& scene, const char* name)
  {
    if (scene.objectsNameIdxTable[name] != INT_MAX)
    {
      return scene.objects[scene.objectsNameIdxTable[name]];
    }
    Logger::LogErr("Accessing invalid object by name!");
    return dummy;
  }

  const SceneObject::Data& GetObject(const Scene::Data& scene, const char* name)
  {
    if (scene.objectsNameIdxTable[name] != INT_MAX)
    {
      return scene.objects[scene.objectsNameIdxTable[name]];
    }
    Logger::LogErr("Accessing invalid object by name!");
    return dummy;
  }

  SceneObject::Data& GetObject(Scene::Data& scene, Entity::id entity)
  {
    if (scene.entityObjectIdxTable[entity] != INT_MAX)
    {
      return scene.objects[scene.entityObjectIdxTable[entity]];
    }
    Logger::LogErr("Accessing invalid object by entity!");
    return dummy;
  }

  const SceneObject::Data& GetObject(const Scene::Data& scene, Entity::id entity)
  {
    if (scene.entityObjectIdxTable[entity] != INT_MAX)
    {
      return scene.objects[scene.entityObjectIdxTable[entity]];
    }
    Logger::LogErr("Accessing invalid object by entity!");
    return dummy;
  }

  void SpawnObject(Scene::Data& scene, const SceneObject::Data& object)
  {
    int index = AddObject(scene, object);
    CreateEntity(scene, index);
    SceneObject::Construct(scene, scene.objects[index]);
    SceneObject::DrawConstruct(scene, scene.objects[index]);
  }

  int AddObject(Scene::Data& scene, const SceneObject::Data& object)
  {
    auto copy = object;

    int count = 0;
    // Want to make sure all names are unique
    while (!ValidateObjectName(scene, copy.name.c_str()))
    {
      copy.name = (String(object.name.c_str()) + String::ToString(count++)).c_str();
    }

    scene.objects.PushBack(copy);
    scene.objectsNameIdxTable[copy.name.c_str()] = static_cast<int>(scene.objects.size - 1);
    return (int)scene.objects.size - 1;
  }

  void RemoveObject(Scene::Data& scene, Entity::id entity)
  {
    size_t i = 0;
    for (auto it = scene.objects.begin(); it != scene.objects.end(); ++it, ++i)
    {
      if (it->entity == entity)
      {
        SceneObject::DrawDestruct(scene, *it);
        SceneObject::Destruct(scene, *it);
        DestroyEntity(scene, *it);
        if (i == scene.objects.size - 1)
        {
          scene.objects.PopBack();
        }
        else
        {
          scene.objects[i] = std::move(scene.objects.back());
          scene.objects.PopBack();
          scene.entityObjectIdxTable[scene.objects[i].entity] = (int)i;
          scene.objectsNameIdxTable[scene.objects[i].name.c_str()] = (int)i;
        }
        return;
      }
    }
  }

  bool ValidateObjectName(const Scene::Data& scene, const char* name)
  {
    return scene.objectsNameIdxTable.Find(name) == SIZE_MAX;
  }

  void UpdateObjectName(Scene::Data& scene, SceneObject::Data& object, const char* name)
  {
    scene.objectsNameIdxTable[name] = scene.objectsNameIdxTable[object.name.c_str()];
    scene.objectsNameIdxTable.Remove(object.name.c_str());
    object.name = name;
  }

  void LoadSceneFnsFromDynamicLibrary(const char* name, SceneFns& sceneFns)
  {
    static GlobalStringHashMap<void*> libHandles;
    void* libHandle;
    if (libHandles.Contains(name))
    {
      libHandle = libHandles[name];
    }
    else
    {
      libHandles[name] = nullptr;
      libHandle = libHandles[name];
    }
    sceneFns = {};
    if (libHandle)
    {
      CloseDynamicLibrary(libHandle);
    }
    libHandle = OpenDynamicLibrary(name);

    auto* CreateDynamic = (void* (*)())GetDynamicLibraryFn(libHandle, "CreateDynamic");
    if (!CreateDynamic)
    {
      Logger::LogErr(String("[Scene] Failed to load CreateDynamic from: ") + name);
      CloseDynamicLibrary(libHandle);
      return;
    }
    SceneFns* copy = static_cast<SceneFns*>(CreateDynamic());
    sceneFns = *copy;
    Scene::Data sceneData;
    sceneFns.UpdateFunc(sceneData, 0);
    delete copy;

    libHandles[name] = libHandle;
  }
}
