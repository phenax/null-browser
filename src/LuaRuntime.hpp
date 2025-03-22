#pragma once

#include <QtCore>
#include <functional>
#include <lua.hpp>

#include "AsyncEventLoop.hpp"
#include "widgets/WebViewStack.hpp"

class LuaRuntime : public QObject {
  Q_OBJECT

public:
  static LuaRuntime *instance() {
    static LuaRuntime inst;
    return &inst;
  }

  void evaluate(QString code);
  QVariant evaluateSync(QString code);

  void loadFile(QString path);

  void stopEventLoop();
  void startEventLoop();

  QVariant getValue(int idx);

  DELEGATE(eventLoop, queueTask, queueTask)
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
  static int lua_onUrlOpen(lua_State *state);
  static int lua_onUrlTabOpen(lua_State *state);
  static int lua_addKeymap(lua_State *state);

private:
  lua_State *state;
  AsyncEventLoop *eventLoop = nullptr;
};
