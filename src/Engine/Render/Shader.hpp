// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "STDPCH.hpp"
#include "String.hpp"
#include "FileSystem.hpp"

namespace Temp::Render
{
#ifdef EDITOR
  namespace EditorShaderIdx
  {
    enum EditorShaderIdx
    {
      EDITORGRID = 0,
      HOVERABLE,
      MAX,
    };
  }
#endif

  namespace ShaderIdx
  {
    enum ShaderIdx
    {
#ifdef EDITOR
      TEST = EditorShaderIdx::MAX,
#else
      TEST = 0,
#endif
      TEXT,
      SPRITE,
      //PARTICLE,
      MAX
    };
  }

  const GlobalDynamicArray<const char*>& ShaderFiles();
  Path GetShadersPath();
  const GlobalDynamicArray<GlobalString>& GlobalShaderFiles();
}