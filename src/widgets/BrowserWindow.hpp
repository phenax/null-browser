#pragma once

#include <QMainWindow>

#include "Configuration.hpp"
#include "utils.hpp"
#include "widgets/Decorations.hpp"
#include "widgets/IWebViewMediator.hpp"
#include "widgets/WebViewStack.hpp"

using WindowId = qsizetype;

class BrowserWindow : public QMainWindow {
  Q_OBJECT

public:
  BrowserWindow(const Configuration &configuration,
                QWebEngineProfile *profile = new QWebEngineProfile, const QStringList &urls = {});

  DEFINE_GETTER(get_id, win_id)
  DEFINE_SETTER(set_id, win_id)
  DELEGATE(webview_stack, current_webview_id, current_webview_id)
  DELEGATE(webview_stack, get_webview_list, get_webview_list)
  DELEGATE(webview_stack, set_search_text, set_search_text)
  DELEGATE(webview_stack, open_devtools, open_devtools)
  DELEGATE(webview_stack, webview_history_back, history_back)
  DELEGATE(webview_stack, webview_history_forward, history_forward)
  DELEGATE(webview_stack, close, close_webview)
  DELEGATE(webview_stack, focus_webview, select_webview)
  DELEGATE(webview_stack, scroll, scroll)
  DELEGATE(webview_stack, scroll_to_top, scroll_to_top)
  DELEGATE(webview_stack, scroll_to_bottom, scroll_to_bottom)
  DELEGATE(decorations, set_enabled, set_decoration_enabled)
  DELEGATE(decorations, get_enabled, get_decoration_enabled)
  DELEGATE(decorations, get_view_id, get_decoration_view_id)

  IWebViewMediator *get_webview_mediator(WebViewId webview_id);
  bool has_webview(WebViewId webview_id);
  void open_url(const QUrl &url, OpenType open_type, WebViewId webview_id);
  void set_html(const QString &html, WebViewId webview_id);
  void run_javascript(const QString &js_code, WebViewId webview_id);
  void expose_rpc_function(const QString &name, const RpcFunc &action, WebViewId webview_id);

  bool on_window_key_event(QObject *target, QKeyEvent *event);

  void closeEvent(QCloseEvent * /*event*/) override { emit closed(); };

  void update_user_agent(const QString &user_agent);
  void update_downloads_dir(const QString &downloads_dir);
  void update_permissions_persistance(const QString &persistance);

signals:
  void closed();
  void new_window_requested(const QUrl &url);
  void close_window_requested();

private:
  const Configuration &configuration;
  WebViewStack *webview_stack;
  Decorations *decorations;

  WindowId win_id = -1;
};

using WindowMap = std::unordered_map<uint64_t, BrowserWindow *>;
