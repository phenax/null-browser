#include <QtCore>
#include <lua.hpp>
extern "C" {
#include <luv.h>
}

#include "LuaRuntime.hpp"

LuaRuntime::LuaRuntime() {
  state = luaL_newstate();
  luaL_openlibs(state);

  // Load `uv` (luv)
  luaopen_luv(state);
  lua_setglobal(state, "uv");

  // Load `web`
  luaL_Reg weblib[] = {
      {"open", &LuaRuntime::lua_onUrlOpen},
      {"tabopen", &LuaRuntime::lua_onUrlTabOpen},
      {NULL, NULL},
  };
  luaL_newlib(state, weblib);
  lua_setglobal(state, "web");

  // auto pp = R"(
  // print('Hello -- ');
  // local h = uv.fs_open('foobar', 'w', tonumber('644', 8), function(err, h)
  //  assert(not err, err);
  //  print('inside');
  //  print(h);
  //  print(err);
  //  uv.fs_write(h, 'Hello world');
  // end);
  // print(h);
  // uv.run();
  // print('-- end');
  // )";
  // auto pp = R"(
  // print('Hello -- ');
  // local t = uv.new_timer();
  // uv.timer_start(t, 4000, 0, function()
  //   print('after time')
  // end);
  // uv.run();
  // print('-- end');
  // )";
  // if (luaL_dostring(state, pp)) {
  //   qDebug() << "Lua Error: " << lua_tostring(state, -1);
  // } else {
  //   qDebug() << "succ";
  // }
}

void LuaRuntime::evaluate(QString code) {
  luaL_dostring(state, code.toStdString().c_str());
}

int LuaRuntime::lua_onUrlOpen(lua_State *state) {
  const char *url = luaL_optstring(state, 1, "");
  auto runtime = LuaRuntime::instance();
  emit runtime->urlOpened(url, OpenType::OpenUrl);
  return 1;
}

int LuaRuntime::lua_onUrlTabOpen(lua_State *state) {
  const char *url = luaL_optstring(state, 1, "");
  auto runtime = LuaRuntime::instance();
  emit runtime->urlOpened(url, OpenType::OpenUrlInTab);
  return 1;
}

LuaRuntime::~LuaRuntime() { lua_close(state); }
