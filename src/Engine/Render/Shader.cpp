// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Array_fwd.hpp"
#include "EngineUtils.hpp"

#include "GameShader.hpp"
#include "String.hpp"

namespace Temp::Render
{
  namespace
  {
    const GlobalDynamicArray<const char*>& InternalShaderFiles()
    {
      static GlobalDynamicArray<const char*> out = {
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

  const GlobalDynamicArray<const char*>& ShaderFiles()
  {
    static const auto& out = InternalShaderFiles();
    return out;
  }

  const std::filesystem::path& GetShadersPath()
  {
    static auto shadersPath = AssetsDirectory() / "Shaders";
    return shadersPath;
  }

  ThreadedDynamicArray<ThreadedString> GlobalShaderFiles()
  {
    ThreadedDynamicArray<ThreadedString> out{(GetShadersPath() / "Common.glsl").c_str()};
    return out;
  }

  GlobalDynamicArray<std::filesystem::file_time_type>& GlobalShaderFilesTimes()
  {
    static GlobalDynamicArray<std::filesystem::file_time_type> out;
    static bool first = true;
    if (first)
    {
      out.Resize(GlobalShaderFiles().size);
      first = false;
    }
    return out;
  }
}