#pragma once

#include <QtCore>
#include <lua.hpp>

#include "events/Event.hpp"
#include "keymap/KeymapEvaluator.hpp"

class ModeChangedEvent : public Event {
public:
  const KeyMode mode;

  ModeChangedEvent(KeyMode mode) : mode(std::move(mode)) { kind = "ModeChanged"; }

  void lua_push(lua_State *state) const override {
    Event::lua_push(state);
    SET_FIELD("mode", string, mode.toStdString().c_str())
  }
};
