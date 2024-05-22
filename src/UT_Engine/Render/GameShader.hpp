// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array.hpp"
#include "Shader.hpp"

namespace Temp::Render
{
  namespace GameShaderIdx
  {
    enum GameShaderIdx
    {
      TEMP = ShaderIdx::MAX,
      MAX
    };
  }

  inline void ExtensionShaderFiles(GlobalDynamicArray<const char*>&)
  {
  }
}