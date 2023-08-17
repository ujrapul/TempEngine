// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Camera.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "OpenGLWrapper.hpp"
#include <vector>

namespace Temp::SceneObject
{
  struct Data;
}

// TODO: Clean up resources, texture etc.
namespace Temp::Component::Drawable
{
  struct Data
  {
    std::vector<float> vertices{};
    std::vector<unsigned int> indices{};
    std::vector<GLuint> buffers{};
    Math::Vec3f offset{};
    Math::Mat4 model{};
    Entity::id entity{Entity::MAX};
    GLuint VAO{};
    GLuint VBO{};
    GLuint EBO{};
    GLuint texture{};
    GLuint shaderProgram{};
    int numInstances{1};
    int indicesSize{0};
    bool visible{true};
    bool disableDepth{false};
    bool updateModel{false};
#ifdef DEBUG
    int shaderIdx{};
#endif

    // Needed for unit test
    bool operator==(const Data& other) const = default;
  };

  inline std::ostream& operator<<(std::ostream& os, const Drawable::Data& drawable)
  {
    os << "Drawable(" << drawable.offset << " " << drawable.model << ")";
    return os;
  }

  constexpr void Scale(Data& drawable, const Math::Vec3f& scale)
  {
    drawable.model = drawable.model.scale(scale);
    drawable.updateModel = true;
  }

  constexpr void SetScale(Data& drawable, const Math::Vec3f& scale)
  {
    drawable.model.setScale(scale);
    drawable.updateModel = true;
  }

  constexpr void Translate(Data& drawable, const Math::Vec3f& translate)
  {
    drawable.model = drawable.model.translate(translate);
    drawable.updateModel = true;
  }

  void Update(Data& drawable);
  void SetTranslate(Data& drawable, const Math::Vec3f& translate);

  // Make sure all API construction happens before render-thread executes!
  void Construct(Data& drawable,
                 int shaderIdx,
                 int bufferDraw = GL_STATIC_DRAW,
                 const std::vector<int>& numOfElements = {3},
                 int vertexStride = 3,
                 int UBO = Camera::UBO());
  void ConstructFont(Data& drawable,
                     int shaderIdx,
                     int bufferDraw = GL_DYNAMIC_DRAW,
                     const std::vector<int>& numOfElements = {4},
                     int vertexStride = 4,
                     int UBO = Camera::FontUBO());
  void Draw(Data& drawable, int polyMode = GL_FILL);
  void DrawUpdate(Scene::Data& scene, SceneObject::Data& object, Data& drawable);
  void UpdateData(Data& drawable, std::vector<float> vertices, std::vector<unsigned int> indices);
  void Destruct(Data& drawable);
  void UpdateFloatBuffer(GLuint buffer, std::vector<float> data, int BufferDraw = GL_STATIC_DRAW);
  void UpdateIndexBuffer(GLuint buffer, //
                         std::vector<unsigned int> data,
                         int BufferDraw = GL_STATIC_DRAW);
  GLuint CreateFloatBuffer(Data& drawable,
                           std::vector<float>& data,
                           int arrayIndex,
                           int numOfElements,
                           int stride,
                           int position = 0);
  GLuint CreateIntBuffer(Data& drawable,
                         std::vector<int>& data,
                         int arrayIndex,
                         int numOfElements,
                         int stride,
                         int position = 0);
  GLuint CreateFloatInstancedBuffer(Data& drawable,
                                    std::vector<float>& data,
                                    int arrayIndex,
                                    int numOfElements,
                                    int stride,
                                    int position = 0);
  GLuint CreateIntInstancedBuffer(Data& drawable,
                                  std::vector<int>& data,
                                  int arrayIndex,
                                  int numOfElements,
                                  int stride,
                                  int position = 0);
  void UpdateVertexIndexBuffers(Data& drawable, int BufferDraw = GL_STATIC_DRAW);
  Math::Vec4f ConvertToLocalSpace(Data& drawable, const Math::Vec4f worldCoords);
}
