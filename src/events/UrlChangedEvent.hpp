#pragma once

#include <QtCore>
#include <lua.hpp>

#include "events/Event.hpp"
#include "widgets/BrowserWindow.hpp"
#include "widgets/WebViewStack.hpp"

class UrlChangedEvent : public Event {
public:
  const QString url;
  const WebViewId webview_id;
  const WindowId win_id;

  UrlChangedEvent(QString url, WebViewId webview_id, WindowId win_id)
      : url(std::move(url)), webview_id(webview_id), win_id(win_id) {
    kind = "UrlChanged";
  }

  void lua_push(lua_State *state) const override {
    Event::lua_push(state);
    SET_FIELD("view_id", integer, webview_id)
    SET_FIELD("win_id", integer, win_id)
    SET_FIELD("url", string, url.toStdString().c_str())
  }
};
