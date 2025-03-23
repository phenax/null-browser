#pragma once

#include <QtCore>
#include <functional>
#include <lua.hpp>

#include "AsyncEventLoop.hpp"
#include "widgets/WebViewStack.hpp"

#define preserveTop(STATE, BODY)                                               \
  int __top = lua_gettop(STATE);                                               \
  BODY;                                                                        \
  lua_settop(STATE, __top);

class LuaRuntime : public QObject {
  Q_OBJECT

public:
  static LuaRuntime *instance() {
    static LuaRuntime inst;
    return &inst;
  }

  void evaluate(QString code);
  void loadFile(QString path);
  QVariant evaluateSync(QString code);

  void stopEventLoop();
  void startEventLoop();
  DELEGATE(eventLoop, queueTask, queueTask)

  QVariant getLuaValue(int idx);
  DEFINE_GETTER(getState, state)

signals:
  void urlOpened(QString url, OpenType openType);
  void evaluationCompleted(QVariant value);
  void evaluationFailed(QString value);
  void keymapAddRequested(QString mode, QString keyseq, std::function<void()>);
  // void outputProduced(QVariant value);

protected:
  LuaRuntime();
  ~LuaRuntime();
  void initWebLib();
  static int lua_onUrlOpen(lua_State *state);
  static int lua_onUrlTabOpen(lua_State *state);
  static int lua_addKeymap(lua_State *state);

private:
  lua_State *state;
  AsyncEventLoop *eventLoop = nullptr;
};
