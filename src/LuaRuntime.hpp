#pragma once

#include <QtCore>
#include <functional>
#include <lua.hpp>

#include "AsyncEventLoop.hpp"
#include "utils.hpp"
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
  static LuaRuntime &instance() {
    static LuaRuntime runtime;
    return runtime;
  }

  void evaluate(const QString &code);
  void load_file(const QString &path);
  QVariant evaluate_sync(const QString &code);

  void stop_event_loop();
  void start_event_loop();
  DELEGATE(event_loop, queue_task, queue_task)

  QVariant get_lua_value(int idx, QVariant default_value = 0);
  DEFINE_GETTER(get_state, state)

signals:
  void evaluation_completed(QVariant value);
  void evaluation_failed(QString value);

  void history_back_requested(WebViewId webview_id, qsizetype history_index);
  void history_forward_requested(WebViewId webview_id, qsizetype history_index);
  void keymap_added(QString mode, QString keyseq, std::function<void()>);
  void url_opened(QString url, OpenType open_type, WebViewId webview_id);
  void webview_closed(WebViewId webview_id);
  void webview_selected(WebViewId webview_id);

protected:
  LuaRuntime();
  ~LuaRuntime() override;
  void init_web_lib();

  // Lua api
  static int lua_history_back(lua_State *state);
  static int lua_history_forward(lua_State *state);
  static int lua_keymap_set(lua_State *state);
  static int lua_open_url(lua_State *state);
  static int lua_tab_close(lua_State *state);
  static int lua_tab_create(lua_State *state);
  static int lua_tab_current(lua_State *state);
  static int lua_tab_list(lua_State *state);
  static int lua_tab_select(lua_State *state);

private:
  lua_State *state;
  AsyncEventLoop *event_loop = nullptr;
};
