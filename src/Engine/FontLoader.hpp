// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "STDPCH.hpp"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include "gl.h"
#endif

namespace Temp::Font
{
  namespace Type
  {
    enum Type
    {
      ARIMO,
      SHPINSCHER,
      MAX
    };
  }

  inline constexpr const char* Fonts[Type::MAX] = {
    "Arimo",
    "SH-Pinscher",
  };

  struct Character
  {
    Math::Vec2ui size{};   // Size of glyph
    Math::Vec2i bearing{}; // Offset from baseline to left/top of glyph
    long int advance{};    // Offset to advance to next glyph
    GLuint texture{};      // ID handle of the glyph texture
    float left{};
    float right{};
    float top{};
    float bottom{};
    float rectRight{};
    float rectBottom{};
    uint32_t atlasWidth{};
    uint32_t atlasHeight{};

    bool operator==(const Character& other) const = default;
  };

  const GlobalArray<Character, 128>& Characters(int type);
  void LoadFont();

  constexpr uint32_t Padding() { return 16; }
}
