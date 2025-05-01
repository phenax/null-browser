#pragma once

#include <QtCore>
#include <lua.hpp>

#define SET_FIELD(NAME, TYPE, VALUE)                                                               \
  lua_pushstring(state, NAME);                                                                     \
  lua_push##TYPE(state, VALUE);                                                                    \
  lua_settable(state, -3);

// Not a huge fan but looks nice
#define SET_FIELD_CLOSURE_WITH_SELF(NAME, CLOSURE)                                                 \
  lua_pushstring(state, NAME);                                                                     \
  lua_pushlightuserdata(state, (void *)this);                                                      \
  lua_pushcclosure(state, CLOSURE, 1);                                                             \
  lua_settable(state, -3);

#define GET_CLOSURE_SELF(TYPE) static_cast<TYPE>(lua_touserdata(state, lua_upvalueindex(1)));

class Event {
public:
  QString kind = "-";

  virtual void lua_push(lua_State *state) const {
    lua_newtable(state);
    SET_FIELD("type", string, kind.toStdString().c_str())
  };
};
