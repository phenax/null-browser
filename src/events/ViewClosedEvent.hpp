#pragma once

#include <QtCore>
#include <lua.hpp>

#include "WebViewData.hpp"
#include "events/Event.hpp"
#include "widgets/BrowserWindow.hpp"

class ViewClosedEvent : public Event {
public:
  const WebViewId view_id;
  const WindowId win_id;

  ViewClosedEvent(WebViewId view_id, WindowId win_id) : view_id(view_id), win_id(win_id) {
    kind = "ViewClosed";
  }

  void lua_push(lua_State *state) const override {
    Event::lua_push(state);
    SET_FIELD("view", integer, view_id)
    SET_FIELD("win", integer, win_id)
  }
};
