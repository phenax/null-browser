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
#include "WindowActionRouter.hpp"

LuaRuntime::LuaRuntime() {
  state = luaL_newstate();
  luaL_openlibs(state);

  preserve_top(state, { init_web_lib(); });

  auto lua_path = QString(PROJECT_LUA_PATH);
  preserve_top(state, {
    lua_getglobal(state, "package");
    lua_getfield(state, -1, "path");
    auto pkg_path = QString(lua_tostring(state, -1)) + ";" + lua_path;
    lua_pop(state, 1);
    lua_pushstring(state, pkg_path.toStdString().c_str());
    lua_setfield(state, -2, "path");

    lua_getglobal(state, "require");
    lua_pushstring(state, "null-browser.api");
    if (lua_pcall(state, 1, 0, 0) != LUA_OK) {
      qCritical() << "Unable to load browser api" << lua_tostring(state, -1);
    }
  });
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

int LuaRuntime::lua_event_register(lua_State *state) {
  EventHandlerRequest event;
  auto top = lua_gettop(state);

  lua_getfield(state, 1, "events");
  auto event_names = LuaRuntime::lua_tostringlist(state);

  event.event_names.swap(event_names);
  if (event.event_names.size() == 0) {
    lua_settop(state, top);
    lua_pushboolean(state, false);
    return 1;
  }

  lua_getfield(state, 1, "patterns");
  auto patterns = LuaRuntime::lua_tostringlist(state);
  event.patterns.swap(patterns);

  lua_getfield(state, 1, "callback");
  if (!lua_isfunction(state, -1)) {
    lua_settop(state, top);
    lua_pushboolean(state, false);
    return 1;
  }

  const int function_ref = luaL_ref(state, LUA_REGISTRYINDEX);
  event.function_ref = function_ref;
  // TODO: Delete ref on clear callback
  event.handler = [state, function_ref](BrowserEvent *event) {
    preserve_top(state, {
      lua_rawgeti(state, LUA_REGISTRYINDEX, function_ref);
      event->lua_push(state);
      lua_pcall(state, 1, 0, 0);
    })
  };

  WindowActionRouter::instance().register_event(event);

  lua_settop(state, top);
  lua_pushboolean(state, true);
  return 1;
}

void LuaRuntime::init_web_lib() {
  // NOLINTBEGIN(modernize-avoid-c-arrays)

  luaL_Reg internals[] = {
      {"register_event", &LuaRuntime::lua_event_register},
      {"set_config", &LuaRuntime::lua_config_set},
      {"get_config", &LuaRuntime::lua_config_get},
      {nullptr, nullptr},
  };
  luaL_newlib(state, internals);
  lua_setglobal(state, internals_global_name);

  // web
  luaL_Reg web[] = {
      /// @deprecated
      {"open", &LuaRuntime::lua_open_url},
      {nullptr, nullptr},
  };
  luaL_newlib(state, web);
  lua_setglobal(state, web_global_name);
  lua_getglobal(state, web_global_name);

  // Keymap api (web.keymap)
  luaL_Reg webkeymap[] = {
      {"set", &LuaRuntime::lua_keymap_set},
      {nullptr, nullptr},
  };
  luaL_newlib(state, webkeymap);
  lua_setfield(state, -2, "keymap");

  // View actions (web.view)
  luaL_Reg webviews[] = {
      {"close", &LuaRuntime::lua_view_close},
      {"new", &LuaRuntime::lua_view_create},
      {"current", &LuaRuntime::lua_view_current},
      {"list", &LuaRuntime::lua_view_list},
      {"select", &LuaRuntime::lua_view_select},
      {"set_url", &LuaRuntime::lua_open_url},
      {nullptr, nullptr},
  };
  luaL_newlib(state, webviews);
  lua_setfield(state, -2, "view");

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

int LuaRuntime::lua_open_url(lua_State *state) {
  const char *url = lua_tostring(state, 1);
  WebViewId view_id = lua_isnoneornil(state, 2) ? 0 : lua_tointeger(state, 2);
  auto &runtime = LuaRuntime::instance();
  emit runtime.url_opened(url, OpenType::OpenUrl, view_id);
  return 1;
}

int LuaRuntime::lua_view_create(lua_State *state) {
  const char *url = luaL_optstring(state, 1, "");
  auto &runtime = LuaRuntime::instance();
  emit runtime.url_opened(url, OpenType::OpenUrlInView, 0);
  return 1;
}

int LuaRuntime::lua_config_set(lua_State *state) {
  const char *key = lua_tostring(state, 1);
  auto &runtime = LuaRuntime::instance();
  QVariant value = runtime.get_lua_value(2, "");
  emit runtime.config_updated(key, value);
  return 1;
}

int LuaRuntime::lua_config_get(lua_State *state) {
  const char *key = lua_tostring(state, 1);
  auto &router = WindowActionRouter::instance();
  auto value = router.fetch_config_value(key);

  if (value.typeId() == QMetaType::QString)
    lua_pushstring(state, value.toString().toStdString().c_str());
  else if (value.typeId() == QMetaType::Int)
    lua_pushinteger(state, value.toInt());
  else if (value.typeId() == QMetaType::Double)
    lua_pushnumber(state, value.toDouble());
  else
    lua_pushnil(state);

  return 1;
}

int LuaRuntime::lua_keymap_set(lua_State *state) {
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

  auto &runtime = LuaRuntime::instance();
  emit runtime.keymap_added(mode, keyseq, action);

  return 1;
}

int LuaRuntime::lua_view_current(lua_State *state) {
  auto &router = WindowActionRouter::instance();
  auto view_id = router.fetch_current_view_id();
  lua_pushinteger(state, view_id);
  return 1;
}

int LuaRuntime::lua_view_list(lua_State *state) {
  auto &router = WindowActionRouter::instance();
  auto views = router.fetch_webview_data_list();
  lua_newtable(state);

  int index = 1; // 1-indexed
  for (auto &view : views) {
    lua_newtable(state);

    lua_pushstring(state, "id");
    lua_pushinteger(state, view.id);
    lua_settable(state, -3);

    lua_pushstring(state, "url");
    lua_pushstring(state, view.url.toStdString().c_str());
    lua_settable(state, -3);

    lua_pushstring(state, "title");
    lua_pushstring(state, view.title.toStdString().c_str());
    lua_settable(state, -3);

    lua_rawseti(state, -2, index);
    index++;
  }

  return 1;
}

int LuaRuntime::lua_history_back(lua_State *state) {
  auto &runtime = LuaRuntime::instance();

  qsizetype history_index = lua_isnoneornil(state, 1) ? 1 : lua_tointeger(state, 1);

  WebViewId view_id = lua_isnoneornil(state, 2) ? 0 : lua_tointeger(state, 2);

  emit runtime.history_back_requested(view_id, history_index);
  return 1;
}

int LuaRuntime::lua_history_forward(lua_State *state) {
  auto &runtime = LuaRuntime::instance();

  qsizetype history_index = lua_isnoneornil(state, 1) ? 1 : lua_tointeger(state, 1);

  WebViewId view_id = lua_isnoneornil(state, 2) ? 0 : lua_tointeger(state, 2);

  emit runtime.history_forward_requested(view_id, history_index);
  return 1;
}

int LuaRuntime::lua_view_close(lua_State *state) {
  auto &runtime = LuaRuntime::instance();

  WebViewId view_id = lua_isnoneornil(state, 1) ? 0 : lua_tointeger(state, 1);

  emit runtime.webview_closed(view_id);
  return 1;
}

int LuaRuntime::lua_view_select(lua_State *state) {
  if (lua_isnoneornil(state, 1))
    return 1; // TODO: return nil (for others too)

  auto &runtime = LuaRuntime::instance();
  WebViewId view_id = lua_tointeger(state, 1);
  emit runtime.webview_selected(view_id);
  return 1;
}

LuaRuntime::~LuaRuntime() {
  stop_event_loop();
  lua_close(state);
  state = nullptr;
}
