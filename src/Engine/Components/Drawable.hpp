// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "STDPCH.hpp" // IWYU pragma: keep
#include "Camera.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "MemoryManager.hpp"
#include "gl.h"

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
    int bufferDraw{GL_STATIC_DRAW};
    bool visible{true};
    bool disableDepth{false};
    int shaderIdx{-1};

    // Needed for unit test
    bool operator==(const Data& other) const = default;
  };

  inline Stream& operator<<(Stream& os, const Drawable::Data& drawable)
  {
    os << "Drawable(\n"
       << "Vertices: " << drawable.vertices
       << "Indices: " << drawable.indices
       << "Buffers: " << drawable.buffers
       << "Offset: " << drawable.offset
       << "Model: " << drawable.model
       << "Entity: " << drawable.entity << "\n"
       << "VAO: " << drawable.VAO << "\n"
       << "VBO: " << drawable.VBO << "\n"
       << "EBO: " << drawable.EBO << "\n"
       << "Texture: " << drawable.texture << "\n"
       << "Shader Program: " << drawable.shaderProgram << "\n"
       << "Num Instances: " << drawable.numInstances << "\n"
       << "Indices Size: " << drawable.indicesSize << "\n"
       << "Buffer Draw: " << drawable.bufferDraw << "\n"
       << "Visible: " << drawable.visible << "\n"
       << "Disable depth: " << drawable.disableDepth << "\n"
       << "Shader Idx: " << drawable.shaderIdx << "\n"
       << ")\n";
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
                     int bufferDraw = GL_STATIC_DRAW,
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
  void UpdateVertexIndexBuffers(Data& drawable);
  Math::Vec4f ConvertToLocalSpace(Data& drawable, Math::Vec4f worldCoords);
}
