// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Math.hpp"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include "gl.h"
#endif
#include <unordered_map>

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

  inline constexpr std::array<const char*, Type::MAX> Fonts{
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
  };

  const std::unordered_map<char, Character>& Characters(int type);
  void LoadFont();

  constexpr uint32_t Padding() { return 16; }
}
