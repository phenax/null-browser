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

signals:
  void history_back_requested(WebViewId webview_id, qsizetype history_index);
  void history_forward_requested(WebViewId webview_id, qsizetype history_index);
  void url_opened(QString url, OpenType open_type, WebViewId webview_id);
  void webview_closed(WebViewId webview_id);
  void webview_selected(WebViewId webview_id);
  void new_window_requested(const QUrl &url);
  void close_window_requested();

public slots:
  void update_user_agent(const QString &user_agent);
  void update_downloads_dir(const QString &downloads_dir);

private:
  WebViewStack *webview_stack;
};
