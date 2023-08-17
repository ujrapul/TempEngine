// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

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

  inline void ExtensionShaderFiles(std::vector<const char*>&)
  {
  }
}