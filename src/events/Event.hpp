#pragma once

#include <QtCore>
#include <lua.hpp>

#define SET_FIELD(NAME, TYPE, VALUE)                                                               \
  lua_pushstring(state, NAME);                                                                     \
  lua_push##TYPE(state, VALUE);                                                                    \
  lua_settable(state, -3);

class Event {
public:
  QString kind = "-";

  virtual void lua_push(lua_State *state) const {
    lua_newtable(state);
    SET_FIELD("type", string, kind.toStdString().c_str())
  };
};
