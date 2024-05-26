// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Drawable.hpp"
#include "Array_fwd.hpp"
#include "Engine.hpp"
#include "EntityType.hpp"
#include "OpenGLWrapper.hpp"
#include "SceneObject.hpp"
#include "TextBox.hpp"

#include "GameDrawable.hpp"

namespace Temp::Component::Drawable
{
  namespace
  {
    void Construct(Data& drawable,
                   int shaderIdx,
                   int bufferDraw,
                   const DynamicArray<int>& numOfElements,
                   int vertexStride,
                   int UBO,
                   const char* UBOMatrices,
                   int UBOMatricesIdx)
    {
      using namespace Temp::Render;

      assert(drawable.entity < Entity::MAX && "Drawable Entity::id not set!");

      drawable.shaderProgram = OpenGLWrapper::GetShaderProgram(shaderIdx);
      if (OpenGLWrapper::globalFreeGLObjects.size > 0)
      {
        OpenGLWrapper::GLObjectData glObject;
        for (auto& glObjectData : OpenGLWrapper::globalFreeGLObjects[shaderIdx])
        {
          if (glObjectData.BufferDraw == bufferDraw)
          {
            glObject = glObjectData;
            break;
          }
        }
        if (glObject.VAO == UINT_MAX)
        {
          drawable.VAO = OpenGLWrapper::CreateVAO();
          drawable.VBO = OpenGLWrapper::CreateVBO(drawable.vertices.buffer,
                                                  drawable.vertices.size,
                                                  bufferDraw);
          drawable.EBO = OpenGLWrapper::CreateEBO(drawable.indices.buffer,
                                                  drawable.indices.size,
                                                  bufferDraw);
        }
        else
        {
          drawable.VAO = glObject.VAO;
          drawable.VBO = glObject.VBO;
          drawable.EBO = glObject.EBO;
          OpenGLWrapper::UpdateVBO(drawable.VBO,
                                  drawable.vertices.buffer,
                                  drawable.vertices.size,
                                  sizeof(float),
                                  bufferDraw);
          OpenGLWrapper::UpdateEBO(drawable.EBO,
                                  drawable.indices.buffer,
                                  drawable.indices.size,
                                  bufferDraw);
          OpenGLWrapper::globalFreeGLObjects[shaderIdx].Remove(glObject);
          glBindVertexArray(drawable.VAO);
          glBindBuffer(GL_ARRAY_BUFFER, drawable.VBO);
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.EBO);
        }
      }
      else
      {
        drawable.VAO = OpenGLWrapper::CreateVAO();
        drawable.VBO = OpenGLWrapper::CreateVBO(drawable.vertices.buffer,
                                                drawable.vertices.size,
                                                bufferDraw);
        drawable.EBO = OpenGLWrapper::CreateEBO(drawable.indices.buffer,
                                                drawable.indices.size,
                                                bufferDraw);
      }
      drawable.indicesSize = (int)drawable.indices.size;
      drawable.bufferDraw = bufferDraw;
      for (size_t i = 0; i < numOfElements.size; ++i)
      {
        OpenGLWrapper::SetVertexAttribArray(i,
                                            numOfElements[i],
                                            vertexStride,
                                            i > 0 ? numOfElements[i - 1] : 0);
      }

      Update(drawable);

      // We are making the assumption that every shader has a Matrices uniform block
      // For getting view and projection from the camera
      OpenGLWrapper::BindUBOShader(UBO, drawable.shaderProgram, UBOMatrices, UBOMatricesIdx);
      OpenGLWrapper::Set1IntShaderProperty(drawable.shaderProgram, "texture0", 0);
      OpenGLWrapper::Set1IntShaderProperty(drawable.shaderProgram, "texture1", 1);

      OpenGLWrapper::Set1IntShaderProperty(drawable.shaderProgram,
                                           "u_numIndices",
                                           drawable.indicesSize);
    }

    template <typename T>
    void CreateBuffer(Data& drawable, DynamicArray<T>& data)
    {
      using namespace Temp::Render::OpenGLWrapper;
      drawable.buffers.PushBack(CreateVBO(data.buffer, sizeof(T), data.size, GL_STATIC_DRAW));
    }

    void MoveFloatBuffer(GLuint buffer, const DynamicArray<float, MemoryManager::Data::SCENE_ARENA>& data, int BufferDraw)
    {
      using namespace Temp::Render::OpenGLWrapper;
      UpdateVBO(buffer, data.buffer, data.size, sizeof(float), BufferDraw);
    }

    void MoveIndexBuffer(GLuint buffer, const DynamicArray<unsigned int, MemoryManager::Data::SCENE_ARENA>& data, int BufferDraw)
    {
      using namespace Temp::Render::OpenGLWrapper;
      UpdateEBO(buffer, data.buffer, data.size, BufferDraw);
    }
  }

  void Update(Data& drawable)
  {
    using namespace Temp::Render;

    OpenGLWrapper::Set4x4MatrixShaderProperty(drawable.shaderProgram,
                                              "model",
                                              &drawable.model.rows[0][0]);
  }

  void Scale(Data& drawable, const Math::Vec3f& scale)
  {
    drawable.model = drawable.model.scale(scale);
  }

  void SetScale(Data& drawable, const Math::Vec3f& scale)
  {
    drawable.model.setScale(scale);
  }

  void Translate(Data& drawable, const Math::Vec3f& translate)
  {
    drawable.model = drawable.model.translate(translate);
  }

  void SetTranslate(Data& drawable, const Math::Vec3f& translate)
  {
    drawable.model.setTranslation(translate + drawable.offset);
  }

  // Make sure all API construction happens before render-thread executes!
  void Construct(Data& drawable,
                 int shaderIdx,
                 int bufferDraw,
                 const DynamicArray<int>& numOfElements,
                 int vertexStride,
                 int UBO)
  {
    using namespace Temp::Render;

    drawable.shaderIdx = shaderIdx;
    Construct(drawable, shaderIdx, bufferDraw, numOfElements, vertexStride, UBO, "Matrices", 0);
  }

  void ConstructFont(Data& drawable,
                     int shaderIdx,
                     int bufferDraw,
                     const DynamicArray<int>& numOfElements,
                     int vertexStride,
                     int UBO)
  {
    using namespace Temp::Render;

    drawable.shaderIdx = shaderIdx;
    Construct(drawable, shaderIdx, bufferDraw, numOfElements, vertexStride, UBO, "FontMatrices", 1);
  }

  void Draw(Scene::Data& scene, SceneObject::Data& object, Data& drawable, int polyMode)
  {
    using namespace Temp::Render;

    if (!drawable.visible)
    {
      return;
    }

    glDepthMask(!drawable.disableDepth); // Don't write into the depth buffer
    glUseProgram(drawable.shaderProgram);
    OpenGLWrapper::BindTexture(GL_TEXTURE0, drawable.texture);
    OpenGLWrapper::Set1FloatShaderProperty(drawable.shaderProgram, "u_time", Global::Time());
    UpdateData(drawable);
    DrawUpdate(scene, object, drawable);
    Update(drawable);

    // Bind the VAO and draw the triangle
    switch (drawable.numInstances)
    {
      case 1:
        OpenGLWrapper::DrawElements(drawable.VAO, drawable.indicesSize, polyMode);
        break;
      default:
        OpenGLWrapper::DrawElementsInstanced(drawable.VAO,
                                             drawable.indicesSize,
                                             drawable.numInstances,
                                             polyMode);
        break;
    }
    glDepthMask(GL_TRUE);
  }

  void DrawUpdate(Scene::Data& scene, SceneObject::Data& object, Data& drawable)
  {
    switch (static_cast<EntityType::EntityType>(object.type))
    {
      case EntityType::TEXTBOX:
        TextBox::DrawUpdate(scene, drawable, *static_cast<TextBox::Data*>(object.data));
        break;
      case EntityType::TEXTBUTTON:
        TextButton::DrawUpdate(scene, drawable, *static_cast<TextButton::Data*>(object.data));
        break;
      case EntityType::SPRITE:
        break;
      case EntityType::MAX:
      case EntityType::MOCK:
        break;
    }
    ExtensionDrawUpdate(scene, object, drawable);
  }

  void UpdateData(Data& drawable)
  {
    drawable.indicesSize = (int)drawable.indices.size;
    if (drawable.shaderProgram == UINT_MAX)
    {
      return;
    }
    Render::OpenGLWrapper::Set1IntShaderProperty(drawable.shaderProgram,
                                                 "u_numIndices",
                                                 drawable.indicesSize);
  }

  void Destruct(Data& drawable)
  {
    using namespace Temp::Render::OpenGLWrapper;

    // IMPORTANT: Make sure to clean up buffers so that vector data can be released and destructed
    // Otherwise memory usage will slowly climb on every new instance of this object
    for (auto buffer : drawable.buffers)
    {
      CleanArrayBuffer(buffer);
    }
    // CleanElementBuffer(drawable.EBO);
    // CleanArrayBuffer(drawable.VBO);
    // CleanArrays(drawable.VAO);

    // CACHE SHADERS SO THAT WE DON'T KEEP ALLOCATING NEW BUFFERS
    globalFreeGLObjects[drawable.shaderIdx].PushBackUnique(
      {drawable.VAO, drawable.VBO, drawable.EBO, drawable.bufferDraw});
    // DONT CLEAN SHADERS THEY WILL EXIST FOR THE ENTIRE PROGRAM
    // CleanShader(drawable.shaderProgram);
    UnbindBuffers();
  }

  void UpdateFloatBuffer(GLuint buffer, DynamicArray<float>& data, int BufferDraw)
  {
    using namespace Temp::Render::OpenGLWrapper;
    UpdateVBO(buffer, data.buffer, data.size, sizeof(float), BufferDraw);
  }

  void UpdateIndexBuffer(GLuint buffer, DynamicArray<unsigned int>& data, int BufferDraw)
  {
    using namespace Temp::Render::OpenGLWrapper;
    UpdateEBO(buffer, data.buffer, data.size, BufferDraw);
  }

  GLuint CreateFloatBuffer(Data& drawable,
                           DynamicArray<float>& data,
                           int arrayIndex,
                           int numOfElements,
                           int stride,
                           int position)
  {
    using namespace Temp::Render::OpenGLWrapper;
    CreateBuffer(drawable, data);
    SetVertexAttribArray(arrayIndex, numOfElements, stride, position);
    return drawable.buffers.back();
  }

  GLuint CreateIntBuffer(Data& drawable,
                         DynamicArray<int>& data,
                         int arrayIndex,
                         int numOfElements,
                         int stride,
                         int position)
  {
    using namespace Temp::Render::OpenGLWrapper;
    CreateBuffer(drawable, data);
    SetVertexIAttribArray(arrayIndex, numOfElements, stride, position);
    return drawable.buffers.back();
  }

  GLuint CreateFloatInstancedBuffer(Data& drawable,
                                    DynamicArray<float>& data,
                                    int arrayIndex,
                                    int numOfElements,
                                    int stride,
                                    int position)
  {
    using namespace Temp::Render::OpenGLWrapper;
    CreateBuffer(drawable, data);
    SetVertexAttribArrayInstanced(arrayIndex, numOfElements, stride, position);
    return drawable.buffers.back();
  }

  GLuint CreateIntInstancedBuffer(Data& drawable,
                                  DynamicArray<int>& data,
                                  int arrayIndex,
                                  int numOfElements,
                                  int stride,
                                  int position)
  {
    using namespace Temp::Render::OpenGLWrapper;
    CreateBuffer(drawable, data);
    SetVertexAttribIArrayInstanced(arrayIndex, numOfElements, stride, position);
    return drawable.buffers.back();
  }

  void UpdateVertexIndexBuffers(Data& drawable)
  {
    MoveFloatBuffer(drawable.VBO, drawable.vertices, drawable.bufferDraw);
    MoveIndexBuffer(drawable.EBO, drawable.indices, drawable.bufferDraw);
  }

  Math::Vec4f ConvertToLocalSpace(Data& drawable, Math::Vec4f worldCoords)
  {
    Math::Mat4 inverseModelMatrix = drawable.model.inverse();
    return inverseModelMatrix * worldCoords;
  }
}
