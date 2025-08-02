#pragma once

#include <QtCore>
#include <lua.hpp>
#include <qevent.h>

#include "events/Event.hpp"

class KeyPressedEvent : public Event {
public:
  const QString key;

  KeyPressedEvent(QString key) : key(std::move(key)) { kind = "KeyPressed"; }

  static KeyPressedEvent *from_qkeyevent(QKeyEvent *qevent) {
    // TODO: Using with modifiers
    auto *event = new KeyPressedEvent(qevent->text());
    return event;
  }

  void lua_push(lua_State *state) const override {
    Event::lua_push(state);
    SET_FIELD("key", string, key.toStdString().c_str())
  }
};
