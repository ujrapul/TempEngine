// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "EngineUtils.hpp"
#include "Math.hpp"
#include "Logger.hpp"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include "gl.h"
#endif
#include <filesystem>
#include <iostream>
#include <vector>

// IMPORTANT NOTES SINCE YOU'RE TOO DUMB TO REMEMBER THEM!
//
// Use glVertexAttribPointer for vec2, GLfloat, GLdouble, and GLubyte
// Use glVertexAttrib"I"Pointer for ivec2, GLint, GLuint, and GLbyte
//

namespace Temp::TGA
{
  struct Header;
}

namespace Temp::Render::OpenGLWrapper
{
  // DO NOT USE OUTSIDE OPENGLWRAPPER!
  inline std::vector<std::vector<const char*>> globalShaders;

  void ClearShaderStrings();
  void LoadShaders();

  inline GLuint CreateShader(const char** shaderSource, int shaderType)
  {
    GLuint shader = glCreateShader(shaderType);
    constexpr int stringArrayCount = 3;
    glShaderSource(shader, stringArrayCount, shaderSource, nullptr);
    glCompileShader(shader);

    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
      GLchar error[128] = {};
      glGetShaderInfoLog(shader, 128, nullptr, error);
      switch (shaderType)
      {
        case GL_VERTEX_SHADER:
          Logger::LogErr("[OpenGL] Vertex shader compilation failed: " + std::string(error));
          break;
        case GL_FRAGMENT_SHADER:
          Logger::LogErr("[OpenGL] Fragment shader compilation failed: " + std::string(error));
          break;
        default:
          break;
      }
      return UINT_MAX;
    }
    return shader;
  }

  constexpr const char** GetShader(int shader) { return globalShaders[shader].data(); }

  inline GLuint CreateVertexShader(int shader)
  {
    return CreateShader(GetShader(shader * 2), GL_VERTEX_SHADER);
  }

  inline GLuint CreateFragmentShader(int shader)
  {
    // We add a '1' since shaders are compiled in two sets.
    return CreateShader(GetShader(shader * 2 + 1), GL_FRAGMENT_SHADER);
  }

  inline GLuint CreateShaderProgram(int shader)
  {
    GLuint vertexShader = CreateVertexShader(shader);
    GLuint fragmentShader = CreateFragmentShader(shader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint status = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
      GLchar error[128] = {};
      glGetProgramInfoLog(shaderProgram, 128, nullptr, error);
      Logger::LogErr("[OpenGL] Shader program linking failed: " + std::string(error));
      return UINT_MAX;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
  }

  inline GLuint CreateVAO()
  {
    GLuint VAO = UINT_MAX;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    return VAO;
  }

  inline GLuint CreateVBO(float* data, size_t arraySize, int BufferDraw = GL_STATIC_DRAW)
  {
    GLuint VBO = UINT_MAX;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arraySize, data, BufferDraw);
    return VBO;
  }

  inline GLuint CreateVBO(void* data,
                          size_t typeSize,
                          size_t arraySize,
                          int BufferDraw = GL_STATIC_DRAW)
  {
    GLuint VBO = UINT_MAX;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, typeSize * arraySize, data, BufferDraw);
    return VBO;
  }

  inline void UpdateVBO(GLuint VBO, void* data, size_t arraySize, int BufferDraw = GL_STATIC_DRAW)
  {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arraySize, data, BufferDraw);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  inline GLuint CreateEBO(GLuint* indices, size_t arraySize, int BufferDraw = GL_STATIC_DRAW)
  {
    // Create Element Buffer Object (EBO) and copy index data
    GLuint EBO = UINT_MAX;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * arraySize, indices, BufferDraw);
    return EBO;
  }

  inline GLuint UpdateEBO(GLuint EBO,
                          GLuint* indices,
                          size_t arraySize,
                          int BufferDraw = GL_STATIC_DRAW)
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * arraySize, indices, BufferDraw);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return EBO;
  }

  inline GLuint CreateUBO(size_t bytes)
  {
    GLuint UBO = UINT_MAX;
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, bytes, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, bytes);
    return UBO;
  }

  inline void BindUBOShader(GLuint UBO, GLuint shaderProgram, const char* property, int index)
  {
    glUniformBlockBinding(shaderProgram, glGetUniformBlockIndex(shaderProgram, property), index);
    glBindBufferBase(GL_UNIFORM_BUFFER, index, UBO);
  }

  inline void UpdateUBO(GLuint UBO, float* data, int size, int offset)
  {
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, (void*)data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  inline void SetVertexAttribArray(int arrayIndex, int numOfElements, int stride, int position)
  {
    glVertexAttribPointer(arrayIndex,
                          numOfElements,
                          GL_FLOAT,
                          GL_FALSE,
                          stride * sizeof(float),
                          (void*)(position * sizeof(float)));
    glEnableVertexAttribArray(arrayIndex);
  }

  inline void SetVertexIAttribArray(int arrayIndex, int numOfElements, int stride, int position)
  {
    glVertexAttribPointer(arrayIndex,
                          numOfElements,
                          GL_INT,
                          GL_FALSE,
                          stride * sizeof(int),
                          (void*)(position * sizeof(int)));
    glEnableVertexAttribArray(arrayIndex);
  }

  // Assumes you're only using float
  inline void SetVertexAttribArrayInstanced(int arrayIndex,
                                            int numOfElements,
                                            int stride,
                                            int position)
  {
    glVertexAttribPointer(arrayIndex,
                          numOfElements,
                          GL_FLOAT,
                          GL_FALSE,
                          stride * sizeof(float),
                          (void*)(position * sizeof(float)));
    glEnableVertexAttribArray(arrayIndex);
    glVertexAttribDivisor(arrayIndex, 1); // Set the attribute to update once per instance
  }

  // Assumes you're only using int
  inline void SetVertexAttribIArrayInstanced(int arrayIndex,
                                             int numOfElements,
                                             int stride,
                                             int position)
  {
    glVertexAttribIPointer(arrayIndex,
                           numOfElements,
                           GL_INT,
                           stride * sizeof(int),
                           (void*)(position * sizeof(int)));
    glEnableVertexAttribArray(arrayIndex);
    glVertexAttribDivisor(arrayIndex, 1); // Set the attribute to update once per instance
  }

  inline void UnbindBuffers()
  {
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  GLuint LoadTextureTGA(const char* texturePath, int imageDataType, GLint filterParam);
  GLuint LoadTextureTGA(const char* texturePath, int imageDataType, TGA::Header& header, GLint filterParam);

  inline GLuint CreateTexture(int imageDataType,
                              int width,
                              int height,
                              void* data,
                              GLint wrapParam,
                              GLint filterParam,
                              int numberOfComponents)
  {
    GLuint texture = UINT_MAX;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 numberOfComponents,
                 width,
                 height,
                 0,
                 imageDataType,
                 GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return texture;
  }

  inline void BindTexture(GLuint textureIndex, GLuint texture)
  {
    glActiveTexture(textureIndex);
    glBindTexture(GL_TEXTURE_2D, texture);
  }

  inline void Set1IntShaderProperty(GLuint shaderProgram, const char* property, int value)
  {
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, property), value);
  }

  inline void Set1FloatShaderProperty(GLuint shaderProgram, const char* property, float value)
  {
    glUseProgram(shaderProgram);
    glUniform1f(glGetUniformLocation(shaderProgram, property), value);
  }

  inline void Set1Vec3ShaderProperty(GLuint shaderProgram,
                                     const char* property,
                                     const Math::Vec3f& value)
  {
    glUseProgram(shaderProgram);
    glUniform3fv(glGetUniformLocation(shaderProgram, property), 1, value.data);
  }

  inline void Set1BoolShaderProperty(GLuint shaderProgram, const char* property, bool value)
  {
    Set1IntShaderProperty(shaderProgram, property, value);
  }

  inline void Set4x4MatrixShaderProperty(GLuint shaderProgram, const char* property, float* matrix)
  {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, property), 1, GL_TRUE, matrix);
  }

  inline void DrawArrays(GLuint vao, int numVertices)
  {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    glBindVertexArray(0);
  }

#ifdef DEBUG
  inline void DrawElements(GLuint vao, int numIndices, int polyMode = GL_FILL)
#else
  inline void DrawElements(GLuint vao, int numIndices, int)
#endif
  {
    glBindVertexArray(vao);
#ifdef DEBUG
    glPolygonMode(GL_FRONT_AND_BACK, polyMode);
#endif
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

#ifdef DEBUG
  inline void DrawElementsInstanced(GLuint vao,
                                    int numIndices,
                                    int numInstances,
                                    int polyMode = GL_FILL)
#else
  inline void DrawElementsInstanced(GLuint vao, int numIndices, int numInstances, int)
#endif
  {
    glBindVertexArray(vao);
#ifdef DEBUG
    glPolygonMode(GL_FRONT_AND_BACK, polyMode);
#endif
    glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0, numInstances);
    glBindVertexArray(0);
  }

  inline void CleanArrays(GLuint& vao)
  {
    glBindVertexArray(vao);
    glDeleteVertexArrays(1, &vao);
  }

  inline void CleanArrayBuffer(GLuint& buffer)
  {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glDeleteBuffers(1, &buffer);
  }

  inline void CleanElementBuffer(GLuint& buffer)
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glDeleteBuffers(1, &buffer);
  }

  inline void CleanShader(GLuint& shader) { glDeleteProgram(shader); }

  // Default is 4, use 1 for Font Textures
  // 1 to disable byte-alignment restriction
  inline void SetUnpackAlignment(int alignment = 4)
  {
    glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
  }

  inline void UpdateSubTexture(int xOffset,
                               int yOffset,
                               int textureWidth,
                               int textureHeight,
                               void* data)
  {
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    xOffset,
                    yOffset,
                    textureWidth,
                    textureHeight,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    data);
  }
}
