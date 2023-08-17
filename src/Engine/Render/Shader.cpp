// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "EngineUtils.hpp"

#include "GameShader.hpp"

namespace Temp::Render
{
  namespace
  {
    const std::vector<const char*>& InternalShaderFiles()
    {
      static std::vector<const char*> out = {
#ifdef EDITOR
        "EditorGrid.glsl",
        "Hoverable.glsl",
#endif
        "Test.glsl",
        "Text.glsl",
        "Sprite.glsl",
      };
      ExtensionShaderFiles(out);
      return out;
    }
  }

  const std::vector<const char*>& ShaderFiles()
  {
    static const auto& out = InternalShaderFiles();
    return out;
  }

  const std::filesystem::path& GetShadersPath()
  {
    static auto shadersPath = AssetsDirectory() / "Shaders";
    return shadersPath;
  }

  const std::vector<std::string>& GlobalShaderFiles()
  {
    static std::vector<std::string> out = {(GetShadersPath() / "Common.glsl").string()};
    return out;
  }

  std::vector<std::filesystem::file_time_type>& GlobalShaderFilesTimes()
  {
    static std::vector<std::filesystem::file_time_type> out;
    static bool first = true;
    if (first)
    {
      out.resize(GlobalShaderFiles().size());
      first = false;
    }
    return out;
  }
}