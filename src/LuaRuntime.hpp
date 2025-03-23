#pragma once

#include <QtCore>
#include <functional>
#include <lua.hpp>

#include "AsyncEventLoop.hpp"
#include "widgets/WebViewStack.hpp"

#define preserve_top(STATE, BODY)                                              \
  const int __top = lua_gettop(STATE);                                         \
  BODY;                                                                        \
  lua_settop(STATE, __top);

class LuaRuntime : public QObject {
  Q_OBJECT

  const char *uv_global_name = "uv";
  const char *web_global_name = "web";

public:
  static LuaRuntime *instance() {
    static LuaRuntime inst;
    return &inst;
  }

  void evaluate(const QString &code);
  void load_file(const QString &path);
  QVariant evaluate_sync(const QString &code);

  void stop_event_loop();
  void start_event_loop();
  DELEGATE(event_loop, queue_task, queue_task)

  QVariant get_lua_value(int idx);
  DEFINE_GETTER(get_state, state)

signals:
  void url_opened(QString url, OpenType open_type);
  void evaluation_completed(QVariant value);
  void evaluation_failed(QString value);
  void keymap_add_requested(QString mode, QString keyseq,
                            std::function<void()>);
  // void output_produced(QVariant value);

protected:
  LuaRuntime();
  ~LuaRuntime() override;
  void init_web_lib();
  static int lua_on_url_open(lua_State *state);
  static int lua_on_url_tab_open(lua_State *state);
  static int lua_add_keymap(lua_State *state);

private:
  lua_State *state;
  AsyncEventLoop *event_loop = nullptr;
};
