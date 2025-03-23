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
  preserveTop(state, { initWebLib(); })
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
  if (eventLoop != nullptr) {
    delete eventLoop;
    eventLoop = nullptr;
  }

  // Clear the uv global
  lua_pushnil(state);
  lua_setglobal(state, uv_global_name);
  lua_gc(state, LUA_GCCOLLECT, 0);
}

void LuaRuntime::evaluate(QString code) {
  eventLoop->queueTask([this, code]() {
    preserveTop(state, {
      if (luaL_dostring(state, code.toStdString().c_str()) != LUA_OK) {
        const char *value = lua_tostring(state, -1);
        qDebug() << "Lua Error: " << value;
        emit evaluationFailed(value);
      } else {
        QVariant value = getLuaValue(-1);
        qDebug() << "result: " << value;
        emit evaluationCompleted(value);
      }
    })
  });
}

QVariant LuaRuntime::evaluateSync(QString code) {
  luaL_dostring(state, code.toStdString().c_str());
  return getLuaValue(-1); // TODO: error handling
}

QVariant LuaRuntime::getLuaValue(int idx) {
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

int LuaRuntime::lua_addKeymap(lua_State *state) {
  const char *mode = lua_tostring(state, 1);
  const char *keyseq = lua_tostring(state, 2);

  lua_pushvalue(state, 3);
  int functionRef = luaL_ref(state, LUA_REGISTRYINDEX);
  auto action = [state, functionRef]() {
    preserveTop(state, {
      lua_rawgeti(state, LUA_REGISTRYINDEX, functionRef);
      if (lua_pcall(state, 0, 0, 0) != LUA_OK) {
        const char *error = lua_tostring(state, -1);
        qDebug() << "Error calling Lua function:" << error;
      }
    })
  };
  // TODO: Cleanup function ref on after keymap clear

  auto runtime = LuaRuntime::instance();
  emit runtime->keymapAddRequested(mode, keyseq, action);

  return 1;
}

void LuaRuntime::loadFile(QString path) {
  queueTask([this, path]() {
    preserveTop(state, {
      qDebug() << "Loading: " << path;
      if (luaL_dofile(state, path.toStdString().c_str()) != LUA_OK) {
        qDebug() << "Load file error:" << lua_tostring(state, -1);
      }
    })
  });
}

void LuaRuntime::initWebLib() {
  // web
  luaL_Reg weblib[] = {
      {"open", &LuaRuntime::lua_onUrlOpen},
      {"tabopen", &LuaRuntime::lua_onUrlTabOpen},
      {nullptr, nullptr},
  };
  luaL_newlib(state, weblib);
  lua_setglobal(state, web_global_name);

  // web.keymap
  lua_getglobal(state, web_global_name);
  luaL_Reg keymaplib[] = {
      {"set", &LuaRuntime::lua_addKeymap},
  };
  luaL_newlib(state, keymaplib);
  lua_setfield(state, -2, "keymap");
  // lua_pop(state, 1);
}

LuaRuntime::~LuaRuntime() {
  stopEventLoop();
  lua_close(state);
  state = nullptr;
}
