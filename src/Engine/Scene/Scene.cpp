// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Scene.hpp"
#include "AudioSystem.hpp"
#include "ComponentType.hpp"
#include "Drawable.hpp"
#include "EditorGrid.hpp"
#include "Hoverable.hpp"
#include "LevelSerializer.hpp"
#include "Logger.hpp"
#include "Luable.hpp"
#include "Math.hpp"
#include "OpenGLWrapper.hpp"
#include "SceneObject.hpp"
#include "Shader.hpp"
#include "ThreadPool.hpp"
#include "Updateable.hpp"
#include "lua.h"
#include <cstddef>
#include <string>

namespace Temp::Scene
{
  namespace
  {
#ifdef EDITOR
    EditorGrid::Data editorGrid;
#endif
    SceneObject::Data dummy;

#ifndef EDITOR
    bool isDeserialized = false;
#endif

    void DequeueRender(Scene::Data& scene)
    {
      while (!scene.renderQueue.empty())
      {
        auto render = scene.renderQueue.front();
        render.func(scene, render.data);
        scene.renderQueue.pop();
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
      scene.objectsNameIdxTable[object.name] = INT_MAX;
      scene.entityObjectIdxTable[object.entity] = INT_MAX;
    }
  }

  void Construct(Data& scene)
  {
    scene.objects.reserve(Entity::MAX);
#ifndef EDITOR
    isDeserialized = LevelSerializer::Deserialize(scene, scene.sceneFns.name + ".level");
    if (isDeserialized)
    {
#else
    EditorGrid::Construct(scene, editorGrid);
#endif
      for (size_t i = 0; i < scene.objects.size(); ++i)
      {
        CreateEntity(scene, (int)i);
        SceneObject::Construct(scene, scene.objects[i]);
      }
#ifndef EDITOR

      scene.sceneFns.ConstructFunc(scene);
    }
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
    scene.objects.clear();
    scene.objectsNameIdxTable.clear();
    scene.entityObjectIdxTable.clear();
    scene.sceneFns.DestructFunc(scene);
    SceneObject::ClearActiveDataCache();
    Entity::Reset(scene.entityData);
  }

  void UpdateLegacy(Data& scene, lua_State* L, float deltaTime)
  {
    auto& updateableArray = Scene::GetComponentArray<Component::Type::UPDATEABLE>(
      scene);
    for (size_t i = 0; i < updateableArray.size; ++i)
    {
      Component::Updateable::Update(scene, updateableArray.array[i], deltaTime);
    }
    auto& luableArray = Scene::GetComponentArray<Component::Type::LUABLE>(scene);
    for (size_t i = 0; i < luableArray.size; ++i)
    {
      auto& luable = luableArray.array[i];
      Component::Luable::LoadScript(luable, L);
      luable.luaExec(L, deltaTime);
    }
    scene.sceneFns.UpdateFunc(scene, deltaTime);
  }

  void Update(Data& scene, float deltaTime)
  {
    auto f = [&scene, deltaTime](Component::Updateable::Data& updateable) {
      Component::Updateable::Update(scene, updateable, deltaTime);
    };
    auto luaF = [deltaTime](Component::Luable::Data& luable, lua_State* L) {
      Component::Luable::LoadScript(luable, L);
      luable.luaExec(L, deltaTime);
    };
    auto& updateableArray = Scene::GetComponentArray<Component::Type::UPDATEABLE>(
      scene);
    ThreadPool::EnqueueForEach(scene.threadPool,
                               updateableArray.array,
                               std::move(f),
                               updateableArray.size);
    ThreadPool::Wait(scene.threadPool);

    ThreadPool::EnqueueLuaForEach(
      scene.threadPool,
      Scene::GetComponentArray<Component::Type::LUABLE>(scene),
      std::move(luaF));
    ThreadPool::Wait(scene.threadPool);

    scene.sceneFns.UpdateFunc(scene, deltaTime);
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

    scene.sceneFns.DrawConstructFunc(scene);
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

    scene.sceneFns.DrawDestructFunc(scene);
  }

  void DrawUpdate(Data& scene) { scene.sceneFns.DrawUpdateFunc(scene); }

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
    ThreadPool::Initialize(scene.threadPool);
  }

  void Destroy(Data& scene)
  {
    Entity::Destruct(scene.entityData);
    ThreadPool::Destruct(scene.threadPool);
  }

  Entity::id CreateEntity(Data& scene) { return Entity::Create(scene.entityData); }

  void DestroyEntity(Data& scene, Entity::id entity) { Entity::Destroy(scene.entityData, entity); }

  void EnqueueRender(Data& scene, RenderFunction func, void* data)
  {
    scene.renderQueue.push({func, data});
  }

  void Draw(Data& scene)
  {
    // NOTE: Below true only for multi-threaded render code
    // Avoid using the Render Queue for real-time updates to avoid flickering!
    DequeueRender(scene);
    auto& drawableArray = Scene::GetComponentArray<Component::Type::DRAWABLE>(scene);
    for (size_t i = 0; i < drawableArray.size; ++i)
    {
      Component::Drawable::Draw(drawableArray.array[i]);
#ifdef EDITOR
      if (drawableArray.array[i].entity == editorGrid.entity)
      {
        continue;
      }
#endif
      Component::Drawable::DrawUpdate(
        scene,
        scene.objects[scene.entityObjectIdxTable[drawableArray.array[i].entity]],
        drawableArray.array[i]);
    }
#ifdef EDITOR
    auto& hoverableArray = Scene::GetComponentArray<Component::Type::HOVERABLE>(scene);
    for (size_t i = 0; i < hoverableArray.size; ++i)
    {
      Component::Drawable::Draw(hoverableArray.array[i].drawable, GL_LINE);
    }
#endif
    Temp::Camera::UpdateOrthoScale(scene, (720.f / Temp::Camera::GetHeight()));
    Temp::Camera::UpdateFontOrthoScale(scene, (720.f / Temp::Camera::GetHeight()));
  }

  void ClearRender(Data& scene) { scene.renderQueue = {}; }

  SceneObject::Data& GetObject(Scene::Data& scene, const std::string& name)
  {
    if (scene.objectsNameIdxTable[name] != INT_MAX)
    {
      return scene.objects[scene.objectsNameIdxTable.at(name)];
    }
    Logger::LogErr("Accessing invalid object by name!");
    return dummy;
  }

  const SceneObject::Data& GetObject(const Scene::Data& scene, const std::string& name)
  {
    if (scene.objectsNameIdxTable.at(name) != INT_MAX)
    {
      return scene.objects[scene.objectsNameIdxTable.at(name)];
    }
    Logger::LogErr("Accessing invalid object by name!");
    return dummy;
  }

  SceneObject::Data& GetObject(Scene::Data& scene, Entity::id entity)
  {
    if (scene.entityObjectIdxTable[entity] != INT_MAX)
    {
      return scene.objects[scene.entityObjectIdxTable.at(entity)];
    }
    Logger::LogErr("Accessing invalid object by entity!");
    return dummy;
  }

  const SceneObject::Data& GetObject(const Scene::Data& scene, Entity::id entity)
  {
    if (scene.entityObjectIdxTable.at(entity) != INT_MAX)
    {
      return scene.objects[scene.entityObjectIdxTable.at(entity)];
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
    while (!ValidateObjectName(scene, copy.name))
    {
      copy.name = object.name + std::to_string(count++);
    }

    scene.objects.push_back(copy);
    scene.objectsNameIdxTable[copy.name] = static_cast<int>(scene.objects.size() - 1);
    return (int)scene.objects.size() - 1;
  }

  void RemoveObject(Scene::Data& scene, const SceneObject::Data& object)
  {
    size_t i = 0;
    for (auto it = scene.objects.begin(); it != scene.objects.end(); ++it, ++i)
    {
      if (it->name == object.name)
      {
        SceneObject::DrawDestruct(scene, *it);
        SceneObject::Destruct(scene, *it);
        DestroyEntity(scene, *it);
        if (i == scene.objects.size() - 1)
        {
          scene.objects.pop_back();
        }
        else
        {
          scene.objects[i] = scene.objects.back();
          scene.objects.pop_back();
          scene.entityObjectIdxTable[scene.objects[i].entity] = (int)i;
          scene.objectsNameIdxTable[scene.objects[i].name] = (int)i;
        }
        return;
      }
    }
  }

  bool ValidateObjectName(const Scene::Data& scene, const std::string& name)
  {
    return scene.objectsNameIdxTable.find(name) == scene.objectsNameIdxTable.end();
  }

  void UpdateObjectName(Scene::Data& scene, SceneObject::Data& object, const std::string& name)
  {
    scene.objectsNameIdxTable[name] = scene.objectsNameIdxTable[object.name];
    scene.objectsNameIdxTable.erase(scene.objectsNameIdxTable.find(object.name));
    object.name = name;
  }
}
