// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "EditorGrid.hpp"
#include "ComponentType.hpp"
#include "Drawable.hpp"
#include "OpenGLWrapper.hpp"
#include "Scene.hpp"
#include "Shader.hpp"
#include <climits>

namespace Temp::EditorGrid
{
  namespace
  {
    // Define the square vertices
    std::vector<float> vertices = {
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
    std::vector<GLuint> indices = {
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
      .vertices = vertices,
      .indices = indices,
      .entity = editorGrid.entity,
    };
    Scene::AddComponent<Component::Type::DRAWABLE>(scene, editorGrid.entity, drawable);
  }

  void DrawConstruct(Scene::Data& scene, Data& editorGrid, int shaderType)
  {
    using namespace Temp::Render;

    auto& drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene, editorGrid.entity);
    Component::Drawable::UpdateData(drawable, vertices, indices);
    Component::Drawable::Construct(drawable, shaderType, GL_STATIC_DRAW, {2}, 2);
    OpenGLWrapper::UnbindBuffers();
  }

  void DrawUpdate(Scene::Data&, Data&) {}

  void DrawDestruct(Scene::Data& scene, Data& editorGrid)
  {
    if (editorGrid.entity != Entity::MAX)
    {
      auto& drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene, editorGrid.entity);
      Component::Drawable::Destruct(drawable);
    }
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