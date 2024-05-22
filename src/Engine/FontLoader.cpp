// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "FontLoader.hpp"

#include "Array_fwd.hpp"
#include "EngineUtils.hpp"
#include "Logger.hpp"
#include "MemoryManager.hpp"
#include "OpenGLWrapper.hpp"
#include "ThreadPool.hpp"
#include "gl.h"
#include <string>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Temp::Font
{
  namespace
  {
    struct GlyphData
    {
      ThreadedDynamicArray<unsigned char> buffer{};
      unsigned int width;
      unsigned int rows;
      int left;
      int top;
      long advanceX;

      bool operator==(const GlyphData& other) const = default;
    };

    constexpr uint32_t padding = Padding();
    constexpr int fontSize = 128;

    constexpr uint32_t asciiNum = 128;
    constexpr uint32_t atlasTileWidth = Math::Floor(Math::Sqrt(asciiNum));
    constexpr uint32_t atlasTileHeight = Math::Ceil(Math::Sqrt(asciiNum));
    constexpr uint32_t atlasWidth = (fontSize + padding * 2) * atlasTileWidth;
    constexpr uint32_t atlasHeight = (fontSize + padding * 2) * atlasTileHeight;
    constexpr uint32_t atlasSize = atlasWidth * atlasHeight;

    typedef ThreadedArray<ThreadedArray<GlyphData, asciiNum>, Type::MAX> GlyphDataMap;
    GlobalArray<GlobalArray<Character, asciiNum>, Type::MAX> fontTable;

    std::string fontFile[Type::MAX] = {
      (std::filesystem::path("Arimo") / "Arimo-Regular.ttf").string(),
      (std::filesystem::path("SHPinscher") / "SHPinscher-Regular.otf").string(),
    };

    void LoadFont(int type, unsigned char c, GlyphDataMap& fontGlyphs)
    {
      FT_Library ft = nullptr;
      if (FT_Init_FreeType(&ft))
      {
        Logger::LogErr(String("ERROR::FREETYPE: Could not init FreeType Library"));
        return;
      }
      std::filesystem::path fontsPath = AssetsDirectory() / "Fonts" / fontFile[type];

      FT_Face face = nullptr;
      if (FT_New_Face(ft, fontsPath.string().c_str(), 0, &face))
      {
        Logger::LogErr(String("ERROR::FREETYPE: Failed to load font > ") + fontsPath.c_str());
        return;
      }

      FT_Set_Pixel_Sizes(face, 0, fontSize);

      if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
      {
        Logger::LogErr(String("ERROR::FREETYTPE: Failed to load Glyph"));
        return;
      }

      // load character glyph
      if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      {
        Logger::LogErr(String("ERROR::FREETYTPE: Failed to load Glyph"));
        return;
      }
      FT_Render_Glyph(face->glyph, FT_RENDER_MODE_SDF); // <-- And this is new
      DynamicArray<unsigned char, MemoryManager::Data::THREAD_TEMP> buffer(
        face->glyph->bitmap.buffer,
        face->glyph->bitmap.buffer + face->glyph->bitmap.width * face->glyph->bitmap.rows);
      fontGlyphs[type][c] = {
        .buffer = std::move(buffer),
        .width = face->glyph->bitmap.width,
        .rows = face->glyph->bitmap.rows,
        .left = face->glyph->bitmap_left,
        .top = face->glyph->bitmap_top,
        .advanceX = face->glyph->advance.x,
      };

      FT_Done_Face(face);
      FT_Done_FreeType(ft);
    }

    void LoadFontTexture(int type,
                         const GlyphDataMap& fontGlyphs,
                         Array<unsigned char, atlasSize>& pixels)
    {
      Render::OpenGLWrapper::SetUnpackAlignment(1);

      uint32_t xOffset = 0;
      uint32_t yOffset = 0;

      // Add an extra pixel offset to fix opengl errors
      // TODO: Fix this to be more accurate
      GLuint texture = Render::OpenGLWrapper::CreateTexture(GL_RED,
                                                            atlasWidth + 1,
                                                            atlasHeight + 1,
                                                            pixels.buffer,
                                                            GL_CLAMP_TO_EDGE,
                                                            GL_NEAREST,
                                                            1);

      for (unsigned char c = 0; c < asciiNum; c++)
      {
        const auto& glyph = fontGlyphs[type][c];
        auto textureWidth = glyph.width;
        auto textureHeight = glyph.rows;

        Render::OpenGLWrapper::UpdateSubTexture(xOffset + padding,
                                                yOffset + padding,
                                                textureWidth,
                                                textureHeight,
                                                glyph.buffer.buffer);

        // now store character for later use
        Character character = {
          {textureWidth + padding * 2, textureHeight + padding * 2},               // size
          {glyph.left, glyph.top},                                                 // bearing
          glyph.advanceX,                                                          // advance
          texture,                                                                 // texture id
          static_cast<float>(xOffset) / atlasWidth,                                // left
          static_cast<float>(xOffset + fontSize) / atlasWidth,                     // right
          static_cast<float>(yOffset) / atlasHeight,                               // top
          static_cast<float>(yOffset + fontSize) / atlasHeight,                    // bottom
          static_cast<float>(xOffset + textureWidth + padding * 2) / atlasWidth,   // rectRight
          static_cast<float>(yOffset + textureHeight + padding * 2) / atlasHeight, // rectBottom
          atlasWidth,
          atlasHeight};
        fontTable[type][c] = character;
        xOffset = (xOffset + fontSize + padding * 2) % atlasWidth;
        if (xOffset == 0)
        {
          yOffset = (yOffset + fontSize + padding * 2) % atlasHeight;
        }
      }

      Render::OpenGLWrapper::SetUnpackAlignment();
    }
  }

  void LoadFont()
  {
    MemoryManager::ScopedTempMemory temp;

    GlyphDataMap fontGlyphs{};
    Array<unsigned char, atlasSize> pixels;

    ThreadPool::Data threadPool;
    ThreadPool::Initialize(threadPool);
    struct TaskData
    {
      GlyphDataMap* fontGlyphs;
      int i;
      unsigned char c;
    };
    DynamicArray<void*> taskDatas;
    for (int i = 0; i < Type::MAX; ++i)
    {
      for (unsigned char c = 0; c < asciiNum; ++c)
      {
        taskDatas.PushBack(MemoryManager::CreateTemp<TaskData>(&fontGlyphs, i, c));
      }
    }
    ThreadPool::EnqueueForEach(threadPool, taskDatas.buffer, [](void* data) {
      auto taskData = static_cast<TaskData*>(data);
      LoadFont(taskData->i, taskData->c, *taskData->fontGlyphs);
    }, taskDatas.size);
    ThreadPool::Wait(threadPool);
    ThreadPool::Destruct(threadPool);
    for (int i = 0; i < Type::MAX; ++i)
    {
      LoadFontTexture(i, fontGlyphs, pixels);
    }
  }

  const Array<Character, asciiNum, MemoryManager::Data::Type::GLOBAL_ARENA>& Characters(int type)
  {
    return fontTable[type];
  }
}
