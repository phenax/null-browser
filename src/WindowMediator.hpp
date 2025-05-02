#pragma once

#include <QWidget>
#include <QtCore>

#include "utils.hpp"
#include "widgets/WebViewStack.hpp"

class WindowMediator : public QObject {
  Q_OBJECT

public:
  WindowMediator(WebViewStack *webview_stack);
  ~WindowMediator() override;

  DELEGATE(webview_stack, has_webview, has_webview)
  DELEGATE(webview_stack, current_webview_id, current_webview_id)
  DELEGATE(webview_stack, get_webview_list, get_webview_list)
  DELEGATE(webview_stack, set_search_text, set_search_text)
  DELEGATE(webview_stack, open_devtools, open_devtools)
  DELEGATE(webview_stack, open_url, open_url)
  DELEGATE(webview_stack, webview_history_back, history_back)
  DELEGATE(webview_stack, webview_history_forward, history_forward)
  DELEGATE(webview_stack, close, close_webview)
  DELEGATE(webview_stack, focus_webview, select_webview)
  DELEGATE(webview_stack, scroll, scroll)
  DELEGATE(webview_stack, scroll_to_top, scroll_to_top)
  DELEGATE(webview_stack, scroll_to_bottom, scroll_to_bottom)

signals:
  void new_window_requested(const QUrl &url);
  void close_window_requested();

public slots:
  void update_user_agent(const QString &user_agent);
  void update_downloads_dir(const QString &downloads_dir);
  void update_permissions_persistance(const QString &persistance);

private:
  WebViewStack *webview_stack;
};
