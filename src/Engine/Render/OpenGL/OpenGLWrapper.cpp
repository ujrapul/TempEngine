// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "OpenGLWrapper.hpp"
#include "MemoryManager.hpp"
#include "Shader.hpp"
#include "String.hpp"
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
    GlobalString COMMON = LoadFileAsString<MemoryManager::Data::GLOBAL_ARENA>(
      (GetShadersPath() / "Common.glsl").c_str());
  }

  void ClearShaderStrings()
  {
    // for (GlobalDynamicArray<const char*>& shaderGroup : globalShaders)
    // {
    //   for (const char* string : shaderGroup)
    //   {
    //     if (string != VERT_HEADER && string != FRAG_HEADER && string != COMMON.c_str())
    //     {
    //       delete[] string;
    //     }
    //   }
    // }
    globalShaders.Clear();
  }

  void LoadShaders()
  {
    ClearShaderStrings();

    const auto& shadersPath = GetShadersPath();

    size_t i = 0;
    for (const auto& shaderFile : ShaderFiles())
    {
      // We're operating on the assumption that these strings won't ever get cleared
      // If we have the memory let them live for the entire lifetime of the program
      globalShaders.InsertEnd(
        {
          {
            VERT_HEADER,
            COMMON.c_str(),
            LoadFileAsString<MemoryManager::Data::GLOBAL_ARENA>((shadersPath / shaderFile).c_str()).c_str(),
          },
          {
            FRAG_HEADER,
            COMMON.c_str(),
            LoadFileAsString<MemoryManager::Data::GLOBAL_ARENA>((shadersPath / shaderFile).c_str()).c_str(),
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
    for (auto& objects : globalFreeGLObjects)
    {
      for (auto& object : objects)
      {
        CleanElementBuffer(object.EBO);
        CleanArrayBuffer(object.VBO);
        CleanArrays(object.VAO);
      }
    }
  }
}
