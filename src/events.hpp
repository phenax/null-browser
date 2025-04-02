#pragma once

#include <QtCore>
#include <lua.hpp>

#include "lua.h"
#include "widgets/BrowserWindow.hpp"
#include "widgets/WebViewStack.hpp"

#define SET_FIELD(NAME, TYPE, VALUE)                                           \
  lua_pushstring(state, NAME);                                                 \
  lua_push##TYPE(state, VALUE);                                                \
  lua_settable(state, -3);

class BrowserEvent {
public:
  std::string kind = "-";
  virtual ~BrowserEvent() = default;

  virtual void lua_push(lua_State *state) { lua_newtable(state); };
};

class UrlChangedEvent : public BrowserEvent {
public:
  const QString url;
  const WebViewId webview_id;
  const WindowId win_id;

  UrlChangedEvent(QString url, WebViewId webview_id, WindowId win_id)
      : url(std::move(url)), webview_id(webview_id), win_id(win_id) {
    kind = "UrlChanged";
  }

  void lua_push(lua_State *state) override {
    lua_newtable(state);
    SET_FIELD("tab", integer, webview_id)
    SET_FIELD("win", integer, win_id)
    SET_FIELD("url", string, url.toStdString().c_str())
  }
};

struct EventHandlerRequest {
  std::vector<std::string> event_names;
  std::vector<std::string> patterns;
  std::function<void(BrowserEvent &)> handler;
  int function_ref;
};
