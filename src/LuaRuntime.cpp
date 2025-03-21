#include <QtCore>
#include <lua.hpp>
extern "C" {
#include <luv.h>
}

#include "AsyncEventLoop.hpp"
#include "LuaRuntime.hpp"

const char *uv_global_name = "uv";
const char *web_global_name = "web";

LuaRuntime::LuaRuntime() {
  state = luaL_newstate();
  luaL_openlibs(state);

  // Load `web`
  luaL_Reg weblib[] = {
      {"open", &LuaRuntime::lua_onUrlOpen},
      {"tabopen", &LuaRuntime::lua_onUrlTabOpen},
      {NULL, NULL},
  };
  luaL_newlib(state, weblib);
  lua_setglobal(state, web_global_name);
}

void LuaRuntime::startEventLoop() {
  if (eventLoop != nullptr)
    stopEventLoop();

  // Init event loop
  eventLoop = new AsyncEventLoop();

  // Load `uv` (luv)
  luv_set_loop(state, eventLoop->getUVLoop());
  luaopen_luv(state);
  lua_setglobal(state, uv_global_name);
}

void LuaRuntime::stopEventLoop() {
  if (eventLoop == nullptr)
    return;
  delete eventLoop;
  eventLoop = nullptr;

  // Clear the uv global
  lua_pushnil(state);
  lua_setglobal(state, uv_global_name);
  lua_gc(state, LUA_GCCOLLECT, 0);
}

void LuaRuntime::evaluate(QString code) {
  eventLoop->queueTask([this, code]() {
    if (luaL_dostring(state, code.toStdString().c_str())) {
      auto value = lua_tostring(state, -1);
      lua_pop(state, 1);

      qDebug() << "Lua Error: " << value;
      emit evaluationFailed(value);
    } else {
      auto value = getValue(-1);
      lua_pop(state, 1);

      qDebug() << "result: " << value;
      emit evaluationCompleted(value);
    }
  });
}

QVariant LuaRuntime::evaluateSync(QString code) {
  auto result = luaL_dostring(state, code.toStdString().c_str());
  return getValue(-1); // TODO: error handling
}

QVariant LuaRuntime::getValue(int idx) {
  if (lua_isstring(state, idx))
    return lua_tostring(state, idx);

  if (lua_isnumber(state, idx))
    return lua_tonumber(state, idx);

  if (lua_isboolean(state, idx))
    return lua_toboolean(state, idx);

  if (lua_isnil(state, idx))
    return 0; // TODO: nil representation

  return lua_tostring(state, idx);
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

LuaRuntime::~LuaRuntime() {
  stopEventLoop();
  lua_close(state);
  state = nullptr;
}
