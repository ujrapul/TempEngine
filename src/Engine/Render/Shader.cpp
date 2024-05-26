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

  Path GetShadersPath()
  {
    static GlobalPath shadersPath(AssetsDirectoryGlobal() / "Shaders");
    return shadersPath.c_str();
  }

  const GlobalDynamicArray<GlobalString>& GlobalShaderFiles()
  {
    static GlobalDynamicArray<GlobalString> out{(GetShadersPath() / "Common.glsl").c_str()};
    return out;
  }
}