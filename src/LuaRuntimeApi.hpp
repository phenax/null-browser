#pragma once

#include <lua.hpp>

#include "LuaRuntime.hpp"
#include "WindowActionRouter.hpp"

int lua_api_open_url(lua_State *state) {
  const char *url = lua_tostring(state, 1);
  WebViewId view_id = lua_isnoneornil(state, 2) ? 0 : lua_tointeger(state, 2);
  auto &runtime = LuaRuntime::instance();
  emit runtime.url_opened(url, OpenType::OpenUrl, view_id);
  return 1;
}

int lua_api_view_create(lua_State *state) {
  const char *url = luaL_optstring(state, 1, "");
  auto &runtime = LuaRuntime::instance();
  emit runtime.url_opened(url, OpenType::OpenUrlInView, 0);
  return 1;
}

int lua_api_config_set(lua_State *state) {
  const char *key = lua_tostring(state, 1);
  auto &runtime = LuaRuntime::instance();
  QVariant value = LuaRuntime::get_lua_value(runtime.get_state(), 2, "");
  emit runtime.config_updated(key, value);
  return 1;
}

int lua_api_config_get(lua_State *state) {
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

int lua_api_keymap_set(lua_State *state) {
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

int lua_api_view_current(lua_State *state) {
  auto &router = WindowActionRouter::instance();
  auto view_id = router.fetch_current_view_id();
  lua_pushinteger(state, view_id);
  return 1;
}

int lua_view_list(lua_State *state) {
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

int lua_history_back(lua_State *state) {
  auto &runtime = LuaRuntime::instance();

  qsizetype history_index = lua_isnoneornil(state, 1) ? 1 : lua_tointeger(state, 1);

  WebViewId view_id = lua_isnoneornil(state, 2) ? 0 : lua_tointeger(state, 2);

  emit runtime.history_back_requested(view_id, history_index);
  return 1;
}

int lua_history_forward(lua_State *state) {
  auto &runtime = LuaRuntime::instance();

  qsizetype history_index = lua_isnoneornil(state, 1) ? 1 : lua_tointeger(state, 1);

  WebViewId view_id = lua_isnoneornil(state, 2) ? 0 : lua_tointeger(state, 2);

  emit runtime.history_forward_requested(view_id, history_index);
  return 1;
}

int lua_view_close(lua_State *state) {
  auto &runtime = LuaRuntime::instance();

  WebViewId view_id = lua_isnoneornil(state, 1) ? 0 : lua_tointeger(state, 1);

  emit runtime.webview_closed(view_id);
  return 1;
}

int lua_view_select(lua_State *state) {
  if (lua_isnoneornil(state, 1))
    return 1; // TODO: return nil (for others too)

  auto &runtime = LuaRuntime::instance();
  WebViewId view_id = lua_tointeger(state, 1);
  emit runtime.webview_selected(view_id);
  return 1;
}

int lua_event_register(lua_State *state) {
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

// NOLINTNEXTLINE
static luaL_Reg internals_api[] = {
    luaL_Reg{"event_add_listener", &lua_event_register},
    luaL_Reg{"config_set", &lua_api_config_set},
    luaL_Reg{"config_get", &lua_api_config_get},
    luaL_Reg{"keymap_set", &lua_api_keymap_set},
    luaL_Reg{"view_close", &lua_view_close},
    luaL_Reg{"view_create", &lua_api_view_create},
    luaL_Reg{"view_current", &lua_api_view_current},
    luaL_Reg{"view_list", &lua_view_list},
    luaL_Reg{"view_select", &lua_view_select},
    luaL_Reg{"view_set_url", &lua_api_open_url},
    luaL_Reg{"history_back", &lua_history_back},
    luaL_Reg{"history_forward", &lua_history_forward},
    luaL_Reg{nullptr, nullptr},
};
