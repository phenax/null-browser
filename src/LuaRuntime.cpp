#include <QtCore>
#include <lua.hpp>
extern "C" {
#include <luv.h>
}

#include "AsyncEventLoop.hpp"
#include "LuaRuntime.hpp"

LuaRuntime::LuaRuntime() {
  state = luaL_newstate();
  luaL_openlibs(state);
  preserve_top(state, { init_web_lib(); })
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

void LuaRuntime::evaluate(const QString &code) {
  queue_task([this, code]() {
    preserve_top(state, {
      if (luaL_dostring(state, code.toStdString().c_str()) != LUA_OK) {
        const char *value = lua_tostring(state, -1);
        qDebug() << "Lua Error: " << value;
        emit evaluation_failed(value);
      } else {
        const QVariant value = get_lua_value(-1);
        qDebug() << "result: " << value;
        emit evaluation_completed(value);
      }
    })
  });
}

QVariant LuaRuntime::evaluate_sync(const QString &code) {
  luaL_dostring(state, code.toStdString().c_str());
  return get_lua_value(-1); // TODO: error handling
}

QVariant LuaRuntime::get_lua_value(int idx, QVariant default_value) {
  if (lua_isnoneornil(state, idx))
    return default_value;

  if (lua_isstring(state, idx))
    return lua_tostring(state, idx);

  if (lua_isnumber(state, idx))
    return lua_tonumber(state, idx);

  if (lua_isboolean(state, idx))
    return lua_toboolean(state, idx);

  return lua_tostring(state, idx);
}

int LuaRuntime::lua_on_url_open(lua_State *state) {
  const char *url = luaL_optstring(state, 1, "");
  auto *runtime = LuaRuntime::instance();
  emit runtime->url_opened(url, OpenType::OpenUrl);
  return 1;
}

int LuaRuntime::lua_on_url_tab_open(lua_State *state) {
  const char *url = luaL_optstring(state, 1, "");
  auto *runtime = LuaRuntime::instance();
  emit runtime->url_opened(url, OpenType::OpenUrlInTab);
  return 1;
}

int LuaRuntime::lua_add_keymap(lua_State *state) {
  const char *mode = lua_tostring(state, 1);
  const char *keyseq = lua_tostring(state, 2);

  lua_pushvalue(state, 3);
  const int function_ref = luaL_ref(state, LUA_REGISTRYINDEX);
  auto action = [state, function_ref]() {
    preserve_top(state, {
      lua_rawgeti(state, LUA_REGISTRYINDEX, function_ref);
      if (lua_pcall(state, 0, 0, 0) != LUA_OK) {
        const char *error = lua_tostring(state, -1);
        qDebug() << "Error calling Lua function:" << error;
      }
    })
  };
  // TODO: Cleanup function ref on after keymap clear

  auto *runtime = LuaRuntime::instance();
  emit runtime->keymap_added(mode, keyseq, action);

  return 1;
}

void LuaRuntime::load_file(const QString &path) {
  queue_task([this, path]() {
    preserve_top(state, {
      qDebug() << "Loading: " << path;
      if (luaL_dofile(state, path.toStdString().c_str()) != LUA_OK) {
        qDebug() << "Load file error:" << lua_tostring(state, -1);
      }
    })
  });
}

void LuaRuntime::init_web_lib() {
  // NOLINTBEGIN(modernize-avoid-c-arrays)

  // web
  luaL_Reg web[] = {
      {"open", &LuaRuntime::lua_on_url_open},
      {nullptr, nullptr},
  };
  luaL_newlib(state, web);
  lua_setglobal(state, web_global_name);
  lua_getglobal(state, web_global_name);

  // Keymap api (web.keymap)
  luaL_Reg webkeymap[] = {
      {"set", &LuaRuntime::lua_add_keymap},
      {nullptr, nullptr},
  };
  luaL_newlib(state, webkeymap);
  lua_setfield(state, -2, "keymap");

  // Tab actions (web.tabs)
  luaL_Reg webtabs[] = {
      {"close", &LuaRuntime::lua_tab_closed},
      {"new", &LuaRuntime::lua_on_url_tab_open},
      {"current", &LuaRuntime::lua_get_current_tab_id},
      {nullptr, nullptr},
  };
  luaL_newlib(state, webtabs);
  lua_setfield(state, -2, "tabs");

  // History navigation
  luaL_Reg webhistory[] = {
      {"back", &LuaRuntime::lua_history_back},
      {"forward", &LuaRuntime::lua_history_forward},
      {nullptr, nullptr},
  };
  luaL_newlib(state, webhistory);
  lua_setfield(state, -2, "history");

  // NOLINTEND(modernize-avoid-c-arrays)
}

int LuaRuntime::lua_get_current_tab_id(lua_State *state) {
  auto *runtime = LuaRuntime::instance();
  auto tab_id = runtime->fetch_current_tab_id();
  lua_pushinteger(state, tab_id);
  return 1;
}

int LuaRuntime::lua_history_back(lua_State *state) {
  auto *runtime = LuaRuntime::instance();

  qsizetype history_index =
      lua_isnoneornil(state, 1) ? 1 : lua_tointeger(state, 1);

  qsizetype tab_id;
  if (lua_isnoneornil(state, 2)) {
    tab_id = runtime->fetch_current_tab_id();
  } else {
    tab_id = lua_tointeger(state, 2);
  }

  emit runtime->history_back_requested(tab_id, history_index);
  return 1;
}

int LuaRuntime::lua_history_forward(lua_State *state) {
  auto *runtime = LuaRuntime::instance();

  qsizetype history_index =
      lua_isnoneornil(state, 1) ? 1 : lua_tointeger(state, 1);

  qsizetype tab_id;
  if (lua_isnoneornil(state, 2)) {
    tab_id = runtime->fetch_current_tab_id();
  } else {
    tab_id = lua_tointeger(state, 2);
  }

  emit runtime->history_forward_requested(tab_id, history_index);
  return 1;
}

int LuaRuntime::lua_tab_closed(lua_State *state) {
  auto *runtime = LuaRuntime::instance();

  qsizetype tab_id;
  if (lua_isnoneornil(state, 2)) {
    tab_id = runtime->fetch_current_tab_id();
  } else {
    tab_id = lua_tointeger(state, 2);
  }

  emit runtime->webview_closed(tab_id);
  return 1;
}

LuaRuntime::~LuaRuntime() {
  stop_event_loop();
  lua_close(state);
  state = nullptr;
}
