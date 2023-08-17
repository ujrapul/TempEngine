// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include <filesystem>
#include <vector>

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
      MAX
    };
  }

  const std::vector<const char*>& ShaderFiles();
  const std::filesystem::path& GetShadersPath();
  const std::vector<std::string>& GlobalShaderFiles();
  std::vector<std::filesystem::file_time_type>& GlobalShaderFilesTimes();
}