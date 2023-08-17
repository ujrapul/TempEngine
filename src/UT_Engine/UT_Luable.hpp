// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "EngineUtils.hpp"
#include "Luable.hpp"
#include "Scene.hpp"
#include "UT_Common.hpp"

namespace Temp::Component::Luable::UnitTests
{
  bool luaExecuted = false;

  void LuaExec(lua_State* L, float) { luaExecuted = ExecFunction("myluafunction", L); }

  void Run()
  {
    auto* L = luaL_newstate();
    luaL_openlibs(L);

    Component::Luable::Data luable{
      .path = "Test.lua",
      .luaExec = LuaExec,
    };

    Assert("Test Load Script", LoadScript(luable, L));

    luable.luaExec(L, 0);
    Assert("Test Lua Function Execution", luaExecuted);

    lua_close(L);
  }
}
