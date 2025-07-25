#pragma once

#include <QtCore>
#include <lua.hpp>

#include "events/Event.hpp"
#include "widgets/BrowserWindow.hpp"

class WinCreatedEvent : public Event {
public:
  const WindowId win_id;

  WinCreatedEvent(WindowId win_id) : win_id(win_id) { kind = "WinCreated"; }

  void lua_push(lua_State *state) const override {
    Event::lua_push(state);
    SET_FIELD("win_id", integer, win_id)
  }
};
