#pragma once
#include <QtCore>
#include <lua.hpp>

#include "widgets/BrowserManager.hpp"

class LuaRuntime : public QObject {
  Q_OBJECT

public:
  static LuaRuntime *instance() {
    static LuaRuntime inst;
    return &inst;
  }

  void evaluate(QString code);

protected:
  LuaRuntime();
  ~LuaRuntime();

signals:
  void urlOpened(QString url, OpenType openType);

protected:
  static int lua_onUrlOpen(lua_State *state);
  static int lua_onUrlTabOpen(lua_State *state);

private:
  lua_State *state;
};
