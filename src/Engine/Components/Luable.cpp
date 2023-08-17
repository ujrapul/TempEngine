// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Luable.hpp"
#include "Engine.hpp"
#include "EngineUtils.hpp"
#include "Logger.hpp"
#include <atomic>
#include <string>

namespace Temp::Component::Luable
{
  bool LoadScript(const Data& luable, lua_State* L)
  {
    if (luaL_loadfile(L, (AssetsDirectory() / "LuaScripts" / luable.path).string().c_str()))
    {
      Logger::LogErr("Something went wrong loading the chunk (syntax error?)");
      Logger::LogErr(lua_tostring(L, -1));
      lua_pop(L, 1);
      return false;
    }
    if (lua_pcall(L, 0, LUA_MULTRET, 0))
    {
      Logger::LogErr("Something went wrong during execution");
      Logger::LogErr(lua_tostring(L, -1));
      lua_pop(L, 1);
      return false;
    }
    return true;
  }

  bool ExecFunction(const char* luaFn, lua_State* L)
  {
    // std::lock_guard<std::mutex> lock(mtx);
    lua_getglobal(L, luaFn);
    if (!lua_isfunction(L, -1))
    {
      Logger::LogErr("Ill-formed Lua functionn: " + std::string(luaFn));
      // Function not found or not callable, handle it gracefully.
      lua_pop(L, 1);
      return false; // or return an error code, depending on your design.
    }
    lua_pushnumber(L, 5);
    if (lua_pcall(L, 1, 1, 0))
    {
      Logger::LogErr("Error calling Lua function: " + std::string(luaFn));
      lua_pop(L, 1);
      return false;
    }
    lua_pop(L, 1);
    return true;
  }
}
