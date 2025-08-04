#include <QtCore>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <lua.hpp>
#include <optional>
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

  init_builtins_package_path();
  init_web_api();
}

int luv_callback(lua_State *state, int nargs, int nresults, int flags) {
  return luv_cfpcall(state, nargs, nresults, flags | LUVF_CALLBACK_NOEXIT);
}

void LuaRuntime::start_event_loop() {
  if (event_loop != nullptr)
    stop_event_loop();

  // Init event loop
  event_loop = new AsyncEventLoop();

  // Load `uv` (luv)
  luv_set_loop(state, event_loop->get_uv_loop());
  luv_set_callback(state, &luv_callback);
  luaopen_luv(state);
  lua_setglobal(state, "uv");
  luaL_dostring(state, "web.uv = uv");
}

void LuaRuntime::stop_event_loop() {
  if (event_loop != nullptr) {
    delete event_loop;
    event_loop = nullptr;
  }

  // Clear the uv global
  lua_pushnil(state);
  lua_setglobal(state, "uv");
  lua_gc(state, LUA_GCCOLLECT, 0);
}

void LuaRuntime::init_web_api() {
  luaL_newlib(state, internals_api);
  lua_setglobal(state, "__internals");

  require_module("null-browser.api");
}

void LuaRuntime::require_module(const QString &module_name) {
  preserve_top(state, {
    lua_getglobal(state, "require");
    lua_pushstring(state, module_name.toStdString().c_str());
    if (lua_pcall(state, 1, 0, 0) != LUA_OK) {
      qCritical() << "Unable to require module" << module_name << ":" << lua_tostring(state, -1);
    }
  });
}

void LuaRuntime::init_builtins_package_path() {
  auto builtins_lua_path = QString(PROJECT_LUA_PATH);
  append_package_path(builtins_lua_path);
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

void LuaRuntime::append_package_path(const QString &path) {
  preserve_top(state, {
    lua_getglobal(state, "package");
    lua_getfield(state, -1, "path");
    auto pkg_path = QString(lua_tostring(state, -1)) + ";" + path;
    lua_pop(state, 1);
    lua_pushstring(state, pkg_path.toStdString().c_str());
    lua_setfield(state, -2, "path");
  });
}

void LuaRuntime::load_file_sync(const QString &path) {
  preserve_top(state, {
    if (luaL_dofile(state, path.toStdString().c_str()) != LUA_OK) {
      lua_error(state);
    }
  });
}

LuaRuntime::~LuaRuntime() {
  stop_event_loop();
  lua_close(state);
  state = nullptr;
}

std::vector<QString> LuaRuntime::lua_tostringlist(lua_State *state) {
  std::vector<QString> values;
  if (!lua_istable(state, -1))
    return values;

  lua_pushnil(state); // First key for lua_next()
  while (lua_next(state, -2) != 0) {
    if (lua_isstring(state, -1))
      values.emplace_back(lua_tostring(state, -1));
    lua_pop(state, 1);
  }
  lua_pop(state, 1);

  return values;
}

void LuaRuntime::inspect_lua_stack(lua_State *state) {
  int top = lua_gettop(state);
  qDebug() << "--- Lua Stack (top: " << top << ") ---\n";

  for (int i = 1; i <= top; i++) {
    int type = lua_type(state, i);
    qDebug() << "  " << i << ": " << lua_typename(state, type);
  }

  qDebug() << "---------------------------\n";
  lua_settop(state, top);
}

QVariant LuaRuntime::get_lua_value(lua_State *state, int idx, QVariant default_value) {
  if (lua_isnoneornil(state, idx))
    return default_value;

  if (lua_isstring(state, idx))
    return lua_tostring(state, idx);

  if (lua_isboolean(state, idx))
    return lua_toboolean(state, idx);

  if (lua_isnumber(state, idx))
    return lua_tonumber(state, idx);

  return lua_tostring(state, idx);
}

void LuaRuntime::push_qvariant(lua_State *state, std::optional<QVariant> opt) {
  if (!opt.has_value()) {
    lua_pushnil(state);
    return;
  }

  auto type = QString(opt.value().typeName());
  // qDebug() << type << opt.value();

  // TODO: QVariantMap
  // TODO: QVariantList

  if (type == "") {
    lua_pushnil(state);
    return;
  }

  if (type == "bool") {
    lua_pushboolean(state, opt.value().toBool());
    return;
  }

  if (type == "int") {
    lua_pushinteger(state, opt.value().toInt());
    return;
  }

  if (type == "double") {
    lua_pushnumber(state, opt.value().toDouble());
    return;
  }

  // String default
  lua_pushstring(state, opt.value().toString().toStdString().c_str());
}
