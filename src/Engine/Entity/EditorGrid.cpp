// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "EditorGrid.hpp"
#include "Array_fwd.hpp"
#include "ComponentType.hpp"
#include "Drawable.hpp"
#include "OpenGLWrapper.hpp"
#include "Scene.hpp"

namespace Temp::EditorGrid
{
  namespace
  {
    // Define the square vertices
    float vertices[8] = {
      // positions      // texture coords
      0.5f,
      0.5f,
      0.5f,
      -0.5f,
      -0.5f,
      -0.5f,
      -0.5f,
      0.5f,
    };

    // Define the indices
    GLuint indices[6] = {
      0,
      1,
      3, // First Triangle
      1,
      2,
      3 // Second Triangle
    };
  }

  void Construct(Scene::Data& scene, Data& editorGrid)
  {
    editorGrid.entity = Scene::CreateEntity(scene);
    Temp::Component::Drawable::Data drawable{
      .vertices = {vertices, 8},
      .indices = {indices, 6},
      .entity = editorGrid.entity,
    };
    Scene::AddComponent<Component::Type::DRAWABLE>(scene, editorGrid.entity, drawable);
  }

  void DrawConstruct(Scene::Data& scene, Data& editorGrid, int shaderType)
  {
    using namespace Temp::Render;

    auto& drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene, editorGrid.entity);
    Component::Drawable::Construct(drawable, shaderType, GL_STATIC_DRAW, {2}, 2);
    Component::Drawable::UpdateData(drawable);
    OpenGLWrapper::UnbindBuffers();
  }

  void DrawUpdate(Scene::Data& scene, Component::Drawable::Data& drawable, Data& editorGrid)
  {
    Component::Drawable::UpdateData(drawable);
  }

  void DrawDestruct(Scene::Data&, Data&)
  {
    // if (editorGrid.entity != Entity::MAX)
    // {
    //   auto& drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene, editorGrid.entity);
    //   Component::Drawable::Destruct(drawable);
    // }
  }

  void Destruct(Scene::Data& scene, Data& editorGrid)
  {
    if (editorGrid.entity != Entity::MAX)
    {
      Scene::DestroyEntity(scene, editorGrid.entity);
    }
    editorGrid.entity = Entity::MAX;
  }
}