// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Drawable.hpp"
#include "Engine.hpp"
#include "EntityType.hpp"
#include "OpenGLWrapper.hpp"
#include "SceneObject.hpp"
#include "TextBox.hpp"

#include "GameDrawable.hpp"
#include <cstddef>

namespace Temp::Component::Drawable
{
  namespace
  {
    void Construct(Data& drawable,
                   int shaderIdx,
                   int bufferDraw,
                   const std::vector<int>& numOfElements,
                   int vertexStride,
                   int UBO,
                   const char* UBOMatrices,
                   int UBOMatricesIdx)
    {
      using namespace Temp::Render;

      assert(drawable.entity < Entity::MAX && "Drawable Entity::id not set!");

      drawable.shaderProgram = OpenGLWrapper::CreateShaderProgram(shaderIdx);
      drawable.VAO = OpenGLWrapper::CreateVAO();
      drawable.VBO = OpenGLWrapper::CreateVBO(drawable.vertices.data(),
                                              drawable.vertices.size(),
                                              bufferDraw);
      drawable.EBO = OpenGLWrapper::CreateEBO(drawable.indices.data(),
                                              drawable.indices.size(),
                                              bufferDraw);
      drawable.indicesSize = (int)drawable.indices.size();
      for (size_t i = 0; i < numOfElements.size(); ++i)
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

      FreeContainer(drawable.vertices);
      FreeContainer(drawable.indices);
    }

    template <typename T>
    void CreateBuffer(Data& drawable, std::vector<T>& data)
    {
      using namespace Temp::Render::OpenGLWrapper;
      drawable.buffers.push_back(CreateVBO(data.data(), sizeof(T), data.size(), GL_STATIC_DRAW));
    }
  }
  void Update(Data& drawable)
  {
    using namespace Temp::Render;

    OpenGLWrapper::Set4x4MatrixShaderProperty(drawable.shaderProgram,
                                              "model",
                                              &drawable.model.rows[0][0]);
  }

  void SetTranslate(Data& drawable, const Math::Vec3f& translate)
  {
    drawable.model.setTranslation(translate + drawable.offset);
    drawable.updateModel = true;
  }

  // Make sure all API construction happens before render-thread executes!
  void Construct(Data& drawable,
                 int shaderIdx,
                 int bufferDraw,
                 const std::vector<int>& numOfElements,
                 int vertexStride,
                 int UBO)
  {
    using namespace Temp::Render;

#ifdef DEBUG
    drawable.shaderIdx = shaderIdx;
#endif
    Construct(drawable, shaderIdx, bufferDraw, numOfElements, vertexStride, UBO, "Matrices", 0);
  }

  void ConstructFont(Data& drawable,
                     int shaderIdx,
                     int bufferDraw,
                     const std::vector<int>& numOfElements,
                     int vertexStride,
                     int UBO)
  {
    using namespace Temp::Render;

#ifdef DEBUG
    drawable.shaderIdx = shaderIdx;
#endif
    Construct(drawable, shaderIdx, bufferDraw, numOfElements, vertexStride, UBO, "FontMatrices", 1);
  }

  void Draw(Data& drawable, int polyMode)
  {
    using namespace Temp::Render;

    if (drawable.updateModel)
    {
      Update(drawable);
      drawable.updateModel = false;
    }

    if (!drawable.visible)
    {
      return;
    }

    glDepthMask(!drawable.disableDepth); // Don't write into the depth buffer
    glUseProgram(drawable.shaderProgram);
    OpenGLWrapper::BindTexture(GL_TEXTURE0, drawable.texture);
    OpenGLWrapper::Set1FloatShaderProperty(drawable.shaderProgram, "u_time", Global::Time());

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

  void UpdateData(Data& drawable, std::vector<float> vertices, std::vector<unsigned int> indices)
  {
    drawable.vertices = std::move(vertices);
    drawable.indices = std::move(indices);
    drawable.indicesSize = (int)drawable.indices.size();
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
    CleanElementBuffer(drawable.EBO);
    CleanArrayBuffer(drawable.VBO);
    CleanArrays(drawable.VAO);
    CleanShader(drawable.shaderProgram);
    UnbindBuffers();

    FreeContainer(drawable.buffers);
    FreeContainer(drawable.vertices);
    FreeContainer(drawable.indices);
  }

  void UpdateFloatBuffer(GLuint buffer, std::vector<float> data, int BufferDraw)
  {
    using namespace Temp::Render::OpenGLWrapper;
    UpdateVBO(buffer, data.data(), data.size(), BufferDraw);
    FreeContainer(data);
  }

  void UpdateIndexBuffer(GLuint buffer, std::vector<unsigned int> data, int BufferDraw)
  {
    using namespace Temp::Render::OpenGLWrapper;
    UpdateEBO(buffer, data.data(), data.size(), BufferDraw);
    FreeContainer(data);
  }

  GLuint CreateFloatBuffer(Data& drawable,
                           std::vector<float>& data,
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
                         std::vector<int>& data,
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
                                    std::vector<float>& data,
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
                                  std::vector<int>& data,
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

  void UpdateVertexIndexBuffers(Data& drawable, int BufferDraw)
  {
    UpdateFloatBuffer(drawable.VBO, std::move(drawable.vertices), BufferDraw);
    UpdateIndexBuffer(drawable.EBO, std::move(drawable.indices), BufferDraw);
  }

  Math::Vec4f ConvertToLocalSpace(Data& drawable, const Math::Vec4f worldCoords)
  {
    Math::Mat4 inverseModelMatrix = drawable.model.inverse();
    return inverseModelMatrix * worldCoords;
  }
}
