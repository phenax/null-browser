#pragma once
#include <QtCore>
#include <lua.hpp>

#include "widgets/WebViewStack.hpp"

class LuaRuntime : public QObject {
  Q_OBJECT

public:
  static LuaRuntime *instance() {
    static LuaRuntime inst;
    return &inst;
  }

  void evaluate(QString code);

signals:
  void urlOpened(QString url, OpenType openType);

protected:
  LuaRuntime();
  ~LuaRuntime();
  static int lua_onUrlOpen(lua_State *state);
  static int lua_onUrlTabOpen(lua_State *state);

private:
  lua_State *state;
};
