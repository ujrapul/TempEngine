// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Sprite.hpp"
#include "Drawable.hpp"
#include "Hoverable.hpp"
#include "Logger.hpp"
#include "Scene.hpp"
#include "TGA.hpp"
#include "OpenGLWrapper.hpp"

namespace Temp::Sprite
{
  namespace
  {
    // clang-format off
    // Define the square vertices
    float vertices[16] = {
      // positions      // texture coords
      1.f, 1.f, 1.0f, 1.0f,   // top right
      1.f, 0.f, 1.0f, 0.0f,  // bottom right
      0.f, 0.f,  0.0f, 0.0f, // bottom left
      0.f, 1.f,  0.0f, 1.0f   // top left
    };

    // Define the indices
    GLuint indices[6] = {
      0, 1, 3, // First Triangle
      1, 2, 3  // Second Triangle
    };
    // clang-format on

#ifdef EDITOR
    void Drag(Scene::Data& scene, Component::Hoverable::Data& hoverable, float x, float y)
    {
      auto& sprite = *static_cast<Sprite::Data*>(hoverable.callbackData);
      auto& drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene, sprite.entity);
      auto& position = Scene::Get<Temp::Component::Type::POSITION2D>(scene, sprite.entity);
      position.x = x;
      position.y = y;
      Component::Drawable::SetTranslate(drawable, {x, y, 0});
      hoverable.model = drawable.model;
    }
#endif
  }

  void Construct(Scene::Data& scene, Data& sprite, ConstructData& ctorData, Entity::id entity)
  {
    sprite.entity = entity;
    Component::Drawable::Data drawable;
    drawable.entity = sprite.entity;
#ifdef EDITOR
    Component::Hoverable::Data hoverable{};
    hoverable.scale = ctorData.scale;
    hoverable.x = ctorData.pos.x;
    hoverable.y = ctorData.pos.y;
    hoverable.Drag = Drag;
    hoverable.isDrag = true;
    hoverable.model = drawable.model;
    hoverable.callbackData = &sprite;
    Scene::AddComponent<Component::Type::HOVERABLE>(scene, sprite.entity, hoverable);
#endif
    Component::Drawable::SetTranslate(drawable, {ctorData.pos.x, ctorData.pos.y, 0});
    Component::Drawable::SetScale(drawable, {ctorData.scale.x, ctorData.scale.y, 0});

    Scene::AddComponent<Component::Type::DRAWABLE>(scene, sprite.entity, drawable);
    Scene::AddComponent<Component::Type::POSITION2D>(scene,
                                                     sprite.entity,
                                                     {ctorData.pos.x, ctorData.pos.y});
    Scene::AddComponent<Component::Type::SCALE>(scene, sprite.entity, ctorData.scale);
  }

  void DrawConstruct(Scene::Data& scene, Data& sprite, int shaderType)
  {
    using namespace Temp::Render;

    auto& drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene, sprite.entity);
    TGA::Header header;
    if ((drawable.texture = OpenGLWrapper::LoadTextureTGA(sprite.fileName.c_str(),
                                                          GL_BGRA,
                                                          header,
                                                          GL_NEAREST)) == UINT_MAX)
    {
      Logger::LogErr("[Sprite] Could not load TGA image!");
    }
#ifdef EDITOR
    auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, sprite.entity);
    hoverable.width = 1;
    hoverable.height = 1;
    Component::Hoverable::ConstructDrawable(hoverable);
#endif
    drawable.vertices = {vertices, 16};
    drawable.indices = {indices, 6};
    Component::Drawable::UpdateData(drawable);

    Component::Drawable::Construct(drawable, //
                                   shaderType,
                                   GL_STATIC_DRAW,
                                   {2, 2},
                                   4);
    OpenGLWrapper::UnbindBuffers();
  }

  // void DrawUpdate(Scene::Data& scene, Component::Drawable::Data& drawable, Data& sprite);

#ifdef EDITOR
  void DrawDestruct(Scene::Data& scene, Data& sprite)
  {
    auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, sprite.entity);
    Component::Hoverable::DestructDrawable(hoverable);
  }
#else
  void DrawDestruct(Scene::Data&, Data&) {}
#endif

  void Destruct(Scene::Data&, Data&) {}
}