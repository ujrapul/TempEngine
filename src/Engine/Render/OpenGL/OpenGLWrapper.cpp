// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "OpenGLWrapper.hpp"
#include "Logger.hpp"
#include "Shader.hpp"
#include "TGA.hpp"
#include "gl.h"

// IMPORTANT NOTES SINCE YOU'RE TOO DUMB TO REMEMBER THEM!
//
// Use glVertexAttribPointer for vec2, GLfloat, GLdouble, and GLubyte
// Use glVertexAttrib"I"Pointer for ivec2, GLint, GLuint, and GLbyte
//

namespace Temp::Render::OpenGLWrapper
{
  namespace
  {
    const char* VERT_HEADER = "#version 330\n#define VERTEX_SHADER\n";
    const char* FRAG_HEADER = "#version 330\n#define FRAGMENT_SHADER\n";

    // Need to delete when passed out
    const char* GetCommonShader()
    {
      const char* common = LoadFileAsString(
        std::filesystem::path(GetShadersPath() / "Common.glsl").string().c_str());
      return common;
    }
  }

  void ClearShaderStrings()
  {
    for (GlobalDynamicArray<const char*>& shaderGroup : globalShaders)
    {
      for (const char* string : shaderGroup)
      {
        if (string != VERT_HEADER && string != FRAG_HEADER)
        {
          delete[] string;
        }
      }
    }
    globalShaders.Clear();
  }

  void LoadShaders()
  {
    ClearShaderStrings();

    const auto& shadersPath = GetShadersPath();

    size_t i = 0;
    for (const auto& shaderFile : ShaderFiles())
    {
      globalShaders.InsertEnd(
        {
          {
            VERT_HEADER,
            GetCommonShader(),
            LoadFileAsString(std::filesystem::path(shadersPath / shaderFile).string().c_str()),
          },
          {
            FRAG_HEADER,
            GetCommonShader(),
            LoadFileAsString(std::filesystem::path(shadersPath / shaderFile).string().c_str()),
          },
        } //
      );
      globalShaderPrograms.PushBack(CreateShaderProgram(i++));
    }
  }

  GLuint LoadTextureTGA(const char* texturePath, int imageDataType, GLint filterParam)
  {
    TGA::Header header;
    return LoadTextureTGA(texturePath, imageDataType, header, filterParam);
  }

  GLuint LoadTextureTGA(const char* texturePath, int imageDataType, TGA::Header& header, GLint filterParam)
  {
    DynamicArray<uint8_t> data;
    if (!TGA::Read((AssetsDirectory() / "Images" / texturePath).c_str(), header, data))
    {
      return UINT_MAX;
    }

    return CreateTexture(imageDataType, //
                         header.width,
                         header.height,
                         data.buffer,
                         GL_REPEAT,
                         filterParam,
                         4);
  }

  void Destruct()
  {
    ClearShaderStrings();
    for (auto shader : globalShaderPrograms)
    {
      CleanShader(shader);
    }
    for (auto& GLObject : globalFreeGLObjects)
    {
      CleanElementBuffer(GLObject.EBO);
      CleanArrayBuffer(GLObject.VBO);
      CleanArrays(GLObject.VAO);
    }
  }
}
