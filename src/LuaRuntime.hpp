#pragma once

#include <QtCore>
#include <functional>
#include <lua.hpp>
#include <optional>

#include "AsyncEventLoop.hpp"
#include "utils.hpp"
#include "widgets/BrowserWindow.hpp"
#include "widgets/Decorations.hpp"
#include "widgets/WebView.hpp"

#ifndef PROJECT_LUA_PATH
#define PROJECT_LUA_PATH ""
#endif

#define preserve_top(STATE, BODY)                                                                  \
  {                                                                                                \
    const int __top = lua_gettop(STATE);                                                           \
    BODY;                                                                                          \
    lua_settop(STATE, __top);                                                                      \
  };

class LuaRuntime : public QObject {
  Q_OBJECT

public:
  static LuaRuntime &instance() {
    static LuaRuntime runtime;
    return runtime;
  }

  void evaluate(const QString &code);
  void load_file_sync(const QString &path);
  void append_package_path(const QString &path);
  void require_module(const QString &module_name);

  void stop_event_loop();
  void start_event_loop();
  DELEGATE(event_loop, queue_task, queue_task)

  DEFINE_GETTER(get_state, state)

signals:
  void evaluation_completed(QVariant value);
  void evaluation_failed(QString value);

  // TODO: Figure out consistent naming for these signals
  void history_back_requested(WebViewId webview_id, qsizetype history_index);
  void history_forward_requested(WebViewId webview_id, qsizetype history_index);
  void keymap_add_requested(QString mode, QString keyseq, std::function<void()> action);
  void keymap_mode_update_requested(const QString &mode);
  void url_opened(QString url, OpenType open_type, WebViewId webview_id);
  void webview_closed(WebViewId webview_id);
  void webview_selected(WebViewId webview_id);
  void config_updated(const QString &key, const QVariant &value);
  void search_requested(const QString &text, WebViewId webview_id);
  void search_next_requested(WebViewId webview_id);
  void search_previous_requested(WebViewId webview_id);
  void devtools_requested(WebViewId webview_id);
  void webview_scroll_requested(WebViewId webview_id, int deltax, int deltay);
  void webview_scroll_top_requested(WebViewId webview_id);
  void webview_scroll_bottom_requested(WebViewId webview_id);
  void decoration_set_enabled(DecorationType type, bool enabled, std::optional<WindowId> win_id);
  void webview_html_set_requested(const QString &html, WebViewId view_id);
  void webview_js_eval_requested(const QString &js_code, WebViewId view_id);
  void schedule_for_next_tick(const std::function<void()> &action);
  void webview_rpc_action_defined(const QString &name, const RpcFunc &action, WebViewId view_id);
  void webview_reload_requested(WebViewId webview_id);

protected:
  LuaRuntime();
  ~LuaRuntime() override;
  void init_builtins_package_path();
  void init_web_api();

private:
  lua_State *state;
  AsyncEventLoop *event_loop = nullptr;

public:
  static void inspect_lua_stack(lua_State *state);
  static std::vector<QString> lua_tostringlist(lua_State *state);
  static QVariant get_lua_value(lua_State *state, int idx, QVariant default_value = 0);
};
