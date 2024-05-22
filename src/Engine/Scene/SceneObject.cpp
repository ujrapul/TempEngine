// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "SceneObject.hpp"
#include "Component.hpp"
#include "ComponentType.hpp"
#include "Drawable.hpp"
#include "Entity.hpp"
#include "Hoverable.hpp"
#include "MemoryManager.hpp"
#include "Scene.hpp"
#include "Updateable.hpp"

#include "GameEntityType.hpp"

namespace Temp::SceneObject
{
  namespace
  {
    struct Cache
    {
      ComponentBits componentBits;
    };

    std::unordered_map<Entity::id, Cache> activeDataCache;

    void InternalDestructUpdate(Data& object) { activeDataCache.erase(object.entity); }

    template <int type, typename T, typename C = Null>
    void Init()
    {
      if constexpr (std::is_same<T, NullObject::Data>::value)
      {
        return;
      }

      if constexpr (std::is_same<C, Null>::value)
      {
        FnTable[FnType::CONSTRUCT][type] = [](auto& scene, auto& object) {
          Construct(scene, *static_cast<T*>(object.data), object.entity);
        };
      }
      else
      {
        FnTable[FnType::CONSTRUCT][type] = [](auto& scene, auto& object) {
          Construct(scene,
                    *static_cast<T*>(object.data),
                    *static_cast<C*>(object.constructData),
                    object.entity);
        };
      }
      FnTable[FnType::DRAWCONSTRUCT][type] = [](auto& scene, auto& object) {
        if (object.shaderType < 0)
          DrawConstruct(scene, *static_cast<T*>(object.data));
        else
          DrawConstruct(scene, *static_cast<T*>(object.data), object.shaderType);
      };
      FnTable[FnType::DRAWDESTRUCT][type] = [](auto& scene, auto& object) {
        DrawDestruct(scene, *static_cast<T*>(object.data));
        if (Test(Scene::ComponentBits(scene, object.entity), Component::Type::DRAWABLE))
        {
          auto& drawable = Scene::Get<Component::Type::DRAWABLE>(scene, object.entity);
          Component::Drawable::Destruct(drawable);
        }
      };
      FnTable[FnType::DESTRUCT][type] = [](auto& scene, auto& object) {
        InternalDestructUpdate(object);
        Destruct(scene, *static_cast<T*>(object.data));
        // Freeing twice for both data and constructData
        // Store prevOffset if we ever try to use this like a stack
        MemoryManager::data.Free(object.allocationType, 0);
        MemoryManager::data.Free(object.allocationType, 0);
      };
#ifdef EDITOR
      CopyTable[type] = [](Data& object) -> Data { return Copy<type>(object); };
#endif
    }

    constexpr uint8_t ENUM_MIN = 0;
    constexpr uint8_t ENUM_MAX = GameEntityType::MAX - 1; // EntityType::MAX - 1;

    template <uint8_t E, uint8_t MAX>
    struct EnumRange
    {
      static void InitEnums()
      {
        Init<E, EntityType::MapToType<E>, EntityType::MapToCtorType<E>>();
        if constexpr (E < MAX)
          EnumRange<E + 1, MAX>::InitEnums();
      }
    };

    template <uint8_t min, uint8_t max>
    void Init()
    {
      EnumRange<min, max>::InitEnums();
    }
  }

  void SetActive(Scene::Data& scene, Data& object, bool active)
  {
    if (active && activeDataCache.contains(object.entity))
    {
      ComponentBits componentBits = activeDataCache[object.entity].componentBits;
      if (Test(componentBits, Component::Type::HOVERABLE))
      {
        Scene::RemoveCacheComponent<Component::Type::HOVERABLE>(scene, object.entity);
      }
      if (Test(componentBits, Component::Type::UPDATEABLE))
      {
        Scene::RemoveCacheComponent<Component::Type::UPDATEABLE>(scene, object.entity);
      }
      if (Test(componentBits, Component::Type::DRAWABLE))
      {
        Scene::RemoveCacheComponent<Component::Type::DRAWABLE>(scene, object.entity);
      }
      activeDataCache.erase(object.entity);
    }
    else if (!active && !activeDataCache.contains(object.entity))
    {
      ComponentBits componentBits = Scene::ComponentBits(scene, object.entity);
      activeDataCache[object.entity] = {
        .componentBits = componentBits,
      };
      if (Test(componentBits, Component::Type::HOVERABLE))
      {
        Scene::AddCacheComponent<Component::Type::HOVERABLE>(scene, object.entity);
      }
      if (Test(componentBits, Component::Type::UPDATEABLE))
      {
        Scene::AddCacheComponent<Component::Type::UPDATEABLE>(scene, object.entity);
      }
      if (Test(componentBits, Component::Type::DRAWABLE))
      {
        Scene::AddCacheComponent<Component::Type::DRAWABLE>(scene, object.entity);
      }
    }
  }

  void Init() { Init<ENUM_MIN, ENUM_MAX>(); }

  void ClearActiveDataCache() { activeDataCache.clear(); }

  void Translate(Scene::Data& scene, const SceneObject::Data& object, const Math::Vec3f& position)
  {
    Math::Mat4 model;
    if (Test(Scene::ComponentBits(scene, object.entity), Component::Type::DRAWABLE))
    {
      auto& drawable = Scene::Get<Component::Type::DRAWABLE>(scene, object.entity);
      Component::Drawable::SetTranslate(drawable, position);
      model = drawable.model;
    }

    if (Test(Scene::ComponentBits(scene, object.entity), Component::Type::HOVERABLE))
    {
      auto& hoverable = Scene::Get<Component::Type::HOVERABLE>(scene, object.entity);
      hoverable.x = position.x;
      hoverable.y = position.y;
      if (Test(Scene::ComponentBits(scene, object.entity), Component::Type::DRAWABLE))
      {
        hoverable.model = model;
#ifdef EDITOR
        Component::Hoverable::UpdateDrawable(hoverable);
#endif
      }
    }

    if (Test(Scene::ComponentBits(scene, object.entity), Component::Type::POSITION2D))
    {
      auto& position2D = Scene::Get<Component::Type::POSITION2D>(scene, object.entity);
      position2D.x = position.x;
      position2D.y = position.y;
    }
  }

  void Translate(Scene::Data& scene, const SceneObject::Data& object, const Math::Vec2f& position)
  {
    Translate(scene, object, {position.x, position.y, 0});
  }

  void Scale(Scene::Data& scene, const SceneObject::Data& object, float scale)
  {
    Math::Mat4 model;
    if (Test(Scene::ComponentBits(scene, object.entity), Component::Type::DRAWABLE))
    {
      auto& drawable = Scene::Get<Component::Type::DRAWABLE>(scene, object.entity);
      Component::Drawable::SetScale(drawable, scale);
      model = drawable.model;
    }

    if (Test(Scene::ComponentBits(scene, object.entity), Component::Type::HOVERABLE))
    {
      auto& hoverable = Scene::Get<Component::Type::HOVERABLE>(scene, object.entity);
      hoverable.scale = {scale, scale};
      if (Test(Scene::ComponentBits(scene, object.entity), Component::Type::DRAWABLE))
      {
        hoverable.model = model;
#ifdef EDITOR
        Component::Hoverable::UpdateDrawable(hoverable);
#endif
      }
    }

    if (Test(Scene::ComponentBits(scene, object.entity), Component::Type::SCALE))
    {
      Scene::Get<Component::Type::SCALE>(scene, object.entity) = {scale, scale};
    }
  }

  void Scale(Scene::Data& scene, const SceneObject::Data& object, const Math::Vec2f& scale)
  {
    Math::Mat4 model;
    if (Test(Scene::ComponentBits(scene, object.entity), Component::Type::DRAWABLE))
    {
      auto& drawable = Scene::Get<Component::Type::DRAWABLE>(scene, object.entity);
      Component::Drawable::SetScale(drawable, {scale.x, scale.y, scale.x});
      model = drawable.model;
    }

    if (Test(Scene::ComponentBits(scene, object.entity), Component::Type::HOVERABLE))
    {
      auto& hoverable = Scene::Get<Component::Type::HOVERABLE>(scene, object.entity);
      hoverable.scale = scale;
      if (Test(Scene::ComponentBits(scene, object.entity), Component::Type::DRAWABLE))
      {
        hoverable.model = model;
#ifdef EDITOR
        Component::Hoverable::UpdateDrawable(hoverable);
#endif
      }
    }

    if (Test(Scene::ComponentBits(scene, object.entity), Component::Type::SCALE))
    {
      Scene::Get<Component::Type::SCALE>(scene, object.entity) = scale;
    }
  }
}
