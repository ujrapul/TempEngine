// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif
#include "lua.hpp"
#include <filesystem>

namespace Temp::Component::Luable
{
  struct Data
  {
    std::string path{};
    void(*luaExec)(lua_State* L, float deltaTime){[](lua_State*, float){}};
  };
  
  // TODO: Perhaps add static functions that are called here
  // Every LuaScript should have these functions
  // Or we should provide error checking for whether those fns exist
  bool LoadScript(const Data& luable, lua_State* L);
  bool ExecFunction(const char* luaFn, lua_State* L);
}
