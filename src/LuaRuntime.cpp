#include <lua.hpp>

#include "LuaRuntime.hpp"

LuaRuntime::LuaRuntime() {
  state = luaL_newstate();
  luaL_openlibs(state);

  luaL_Reg weblib[] = {
      {"open", &LuaRuntime::lua_onUrlOpen},
      {"tabopen", &LuaRuntime::lua_onUrlTabOpen},
      {NULL, NULL},
  };
  luaL_newlib(state, weblib);
  lua_setglobal(state, "web");
}

void LuaRuntime::evaluate(const char *code) { luaL_dostring(state, code); }

int LuaRuntime::lua_onUrlOpen(lua_State *state) {
  const char *url = luaL_optstring(state, 1, "");
  auto runtime = LuaRuntime::instance();
  emit runtime->urlOpenned(url, OpenType::OpenUrl);
  return 1;
}

int LuaRuntime::lua_onUrlTabOpen(lua_State *state) {
  const char *url = luaL_optstring(state, 1, "");
  auto runtime = LuaRuntime::instance();
  emit runtime->urlOpenned(url, OpenType::OpenUrlInTab);
  return 1;
}

LuaRuntime::~LuaRuntime() { lua_close(state); }
