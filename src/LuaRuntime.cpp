#include <QtCore>
#include <cstdlib>
#include <cstring>
#include <lua.hpp>
#include <qvariant.h>
extern "C" {
#include <luv/luv.h>
}

#include "AsyncEventLoop.hpp"
#include "LuaRuntime.hpp"
#include "LuaRuntimeApi.hpp"

LuaRuntime::LuaRuntime() {
  state = luaL_newstate();
  luaL_openlibs(state);

  init_lua_package_path();
  init_web_api();
}

void LuaRuntime::start_event_loop() {
  if (event_loop != nullptr)
    stop_event_loop();

  // Init event loop
  event_loop = new AsyncEventLoop();

  // Load `uv` (luv)
  luv_set_loop(state, event_loop->get_uv_loop());
  luaopen_luv(state);
  lua_setglobal(state, uv_global_name);
}

void LuaRuntime::stop_event_loop() {
  if (event_loop != nullptr) {
    delete event_loop;
    event_loop = nullptr;
  }

  // Clear the uv global
  lua_pushnil(state);
  lua_setglobal(state, uv_global_name);
  lua_gc(state, LUA_GCCOLLECT, 0);
}

void LuaRuntime::init_web_api() {
  luaL_newlib(state, internals_api);
  lua_setglobal(state, internals_global_name);

  preserve_top(state, {
    lua_getglobal(state, "require");
    lua_pushstring(state, "null-browser.api");
    if (lua_pcall(state, 1, 0, 0) != LUA_OK) {
      qCritical() << "Unable to load browser api" << lua_tostring(state, -1);
    }
  });
}

void LuaRuntime::init_lua_package_path() {
  auto lua_path = QString(PROJECT_LUA_PATH);
  preserve_top(state, {
    lua_getglobal(state, "package");
    lua_getfield(state, -1, "path");
    auto pkg_path = QString(lua_tostring(state, -1)) + ";" + lua_path;
    lua_pop(state, 1);
    lua_pushstring(state, pkg_path.toStdString().c_str());
    lua_setfield(state, -2, "path");
  });
}

void LuaRuntime::evaluate(const QString &code) {
  queue_task([this, code]() {
    preserve_top(state, {
      if (luaL_dostring(state, code.toStdString().c_str()) != LUA_OK) {
        const char *value = lua_tostring(state, -1);
        qDebug() << "Lua Error: " << value;
        emit evaluation_failed(value);
      } else {
        const QVariant value = get_lua_value(state, -1);
        emit evaluation_completed(value);
      }
    })
  });
}

void LuaRuntime::load_file_sync(const QString &path) {
  preserve_top(state, {
    if (luaL_dofile(state, path.toStdString().c_str()) != LUA_OK) {
      qDebug() << "Load file error:" << lua_tostring(state, -1);
    }
  });
}

LuaRuntime::~LuaRuntime() {
  stop_event_loop();
  lua_close(state);
  state = nullptr;
}
