// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Camera.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "MemoryManager.hpp"
#include "OpenGLWrapper.hpp"

namespace Temp::SceneObject
{
  struct Data;
}

// TODO: Clean up resources, texture etc.
namespace Temp::Component::Drawable
{
  struct Data
  {
    DynamicArray<float, MemoryManager::Data::SCENE_ARENA> vertices{};
    DynamicArray<unsigned int, MemoryManager::Data::SCENE_ARENA> indices{};
    DynamicArray<GLuint, MemoryManager::Data::SCENE_ARENA> buffers{};
    Math::Vec3f offset{};
    Math::Mat4 model{};
    Entity::id entity{Entity::MAX};
    GLuint VAO{};
    GLuint VBO{};
    GLuint EBO{};
    GLuint texture{};
    GLuint shaderProgram{UINT_MAX};
    int numInstances{1};
    int indicesSize{0};
    bool visible{true};
    bool disableDepth{false};
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

  void Update(Data& drawable);
  
  void Scale(Data& drawable, const Math::Vec3f& scale);
  void SetScale(Data& drawable, const Math::Vec3f& scale);
  void Translate(Data& drawable, const Math::Vec3f& translate);
  void SetTranslate(Data& drawable, const Math::Vec3f& translate);

  // Make sure all API construction happens before render-thread executes!
  void Construct(Data& drawable,
                 int shaderIdx,
                 int bufferDraw = GL_STATIC_DRAW,
                 const DynamicArray<int>& numOfElements = {3},
                 int vertexStride = 3,
                 int UBO = Camera::UBO());
  void ConstructFont(Data& drawable,
                     int shaderIdx,
                     int bufferDraw = GL_DYNAMIC_DRAW,
                     const DynamicArray<int>& numOfElements = {4},
                     int vertexStride = 4,
                     int UBO = Camera::FontUBO());
  void Draw(Scene::Data& scene, SceneObject::Data& object, Data& drawable, int polyMode = GL_FILL);
  void DrawUpdate(Scene::Data& scene, SceneObject::Data& object, Data& drawable);
  void UpdateData(Data& drawable);
  void Destruct(Data& drawable);
  void UpdateFloatBuffer(GLuint buffer, DynamicArray<float>& data, int BufferDraw = GL_STATIC_DRAW);
  void UpdateIndexBuffer(GLuint buffer, //
                         DynamicArray<unsigned int>& data,
                         int BufferDraw = GL_STATIC_DRAW);
  GLuint CreateFloatBuffer(Data& drawable,
                           DynamicArray<float>& data,
                           int arrayIndex,
                           int numOfElements,
                           int stride,
                           int position = 0);
  GLuint CreateIntBuffer(Data& drawable,
                         DynamicArray<int>& data,
                         int arrayIndex,
                         int numOfElements,
                         int stride,
                         int position = 0);
  GLuint CreateFloatInstancedBuffer(Data& drawable,
                                    DynamicArray<float>& data,
                                    int arrayIndex,
                                    int numOfElements,
                                    int stride,
                                    int position = 0);
  GLuint CreateIntInstancedBuffer(Data& drawable,
                                  DynamicArray<int>& data,
                                  int arrayIndex,
                                  int numOfElements,
                                  int stride,
                                  int position = 0);
  void UpdateVertexIndexBuffers(Data& drawable, int BufferDraw = GL_STATIC_DRAW);
  Math::Vec4f ConvertToLocalSpace(Data& drawable, Math::Vec4f worldCoords);
}
