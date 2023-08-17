// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "FontLoader.hpp"

#include "EngineUtils.hpp"
#include "Logger.hpp"
#include "Math.hpp"
#include "OpenGLWrapper.hpp"
#include "ThreadPool.hpp"
#include "gl.h"
#include <array>
#include <filesystem>
#include <iostream>
#include <thread>
#include <unordered_map>
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
      std::vector<unsigned char> buffer;
      unsigned int width;
      unsigned int rows;
      int left;
      int top;
      long advanceX;
    };

    std::unordered_map<int, std::unordered_map<char, Character>> fontTable;
    std::unordered_map<int, std::unordered_map<char, GlyphData>> fontGlyphs;
    std::unordered_map<int, std::string> fontFile = {
      {Type::ARIMO, (std::filesystem::path("Arimo") / "Arimo-Regular.ttf").string()},
      {Type::SHPINSCHER, (std::filesystem::path("SHPinscher") / "SHPinscher-Regular.otf").string()},
    };
    std::vector<unsigned char> pixels;

    constexpr uint32_t padding = Padding();
    constexpr int fontSize = 128;

    constexpr uint32_t asciiNum = 128;
    constexpr uint32_t atlasTileWidth = Math::Floor(Math::Sqrt(asciiNum));
    constexpr uint32_t atlasTileHeight = Math::Ceil(Math::Sqrt(asciiNum));
    constexpr uint32_t atlasWidth = (fontSize + padding * 2) * atlasTileWidth;
    constexpr uint32_t atlasHeight = (fontSize + padding * 2) * atlasTileHeight;
    constexpr uint32_t atlasSize = atlasWidth * atlasHeight;

    void LoadFont(int type, unsigned char c)
    {
      FT_Library ft = nullptr;
      if (FT_Init_FreeType(&ft))
      {
        Logger::LogErr("ERROR::FREETYPE: Could not init FreeType Library");
        return;
      }
      std::filesystem::path fontsPath = AssetsDirectory() / "Fonts" / fontFile[type];

      FT_Face face = nullptr;
      if (FT_New_Face(ft, fontsPath.string().c_str(), 0, &face))
      {
        Logger::LogErr("ERROR::FREETYPE: Failed to load font");
        return;
      }

      FT_Set_Pixel_Sizes(face, 0, fontSize);

      if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
      {
        Logger::LogErr("ERROR::FREETYTPE: Failed to load Glyph");
        return;
      }

      // load character glyph
      if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      {
        Logger::LogErr("ERROR::FREETYTPE: Failed to load Glyph");
        return;
      }
      FT_Render_Glyph(face->glyph, FT_RENDER_MODE_SDF); // <-- And this is new
      std::vector<unsigned char> buffer(
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

    void LoadFontTexture(int type)
    {
      Render::OpenGLWrapper::SetUnpackAlignment(1);

      uint32_t xOffset = 0;
      uint32_t yOffset = 0;

      GLuint texture = Render::OpenGLWrapper::CreateTexture(GL_RED,
                                                            atlasWidth,
                                                            atlasHeight,
                                                            pixels.data(),
                                                            GL_CLAMP_TO_EDGE,
                                                            GL_NEAREST,
                                                            1);

      for (unsigned char c = 0; c < asciiNum; c++)
      {
        auto& glyph = fontGlyphs[type][c];
        auto textureWidth = glyph.width;
        auto textureHeight = glyph.rows;

        Render::OpenGLWrapper::UpdateSubTexture(xOffset + padding,
                                                yOffset + padding,
                                                textureWidth,
                                                textureHeight,
                                                glyph.buffer.data());

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
    // We used a gigantic array before which came out to be around 2 mil+ items...
    // Don't do that...
    pixels.resize(atlasSize);

    // Must initialize data in unordered_map to
    // prevent data races when loading font data
    for (int i = 0; i < Type::MAX; ++i)
    {
      fontTable[i] = {};
      fontGlyphs[i] = {};
      for (unsigned char j = 0; j < asciiNum; ++j)
      {
        fontTable[i][j] = {};
        fontGlyphs[i][j] = {};
      }
    }

    ThreadPool::Data threadPool;
    ThreadPool::Initialize(threadPool);
    for (int i = 0; i < Type::MAX; ++i)
    {
      for (unsigned char c = 0; c < asciiNum; ++c)
      {
        ThreadPool::Enqueue(threadPool, [i, c]() { LoadFont(i, c); });
      }
    }
    ThreadPool::Wait(threadPool);
    ThreadPool::Destruct(threadPool);
    for (int i = 0; i < Type::MAX; ++i)
    {
      LoadFontTexture(i);
    }

    FreeContainer(fontGlyphs);
  }

  const std::unordered_map<char, Character>& Characters(int type) { return fontTable[type]; }
}
