#include <QList>
#include <QWidget>
#include <QtCore>
#include <qwebengineprofile.h>

#include "WindowMediator.hpp"
#include "widgets/WebViewStack.hpp"

WindowMediator::WindowMediator(WebViewStack *webview_stack) : webview_stack(webview_stack) {
  connect(this, &WindowMediator::history_back_requested, webview_stack,
          &WebViewStack::webview_history_back);
  connect(this, &WindowMediator::history_forward_requested, webview_stack,
          &WebViewStack::webview_history_forward);

  connect(this, &WindowMediator::url_opened, webview_stack, &WebViewStack::open_url);
  connect(this, &WindowMediator::webview_closed, webview_stack, &WebViewStack::close);
  connect(this, &WindowMediator::webview_selected, webview_stack, &WebViewStack::focus_webview);

  // Delegate signal
  connect(webview_stack, &WebViewStack::new_window_requested, this,
          &WindowMediator::new_window_requested);
  connect(webview_stack, &WebViewStack::close_window_requested, this,
          &WindowMediator::close_window_requested);
}

void WindowMediator::update_user_agent(const QString &user_agent) {
  auto *profile = webview_stack->get_profile();
  profile->setHttpUserAgent(user_agent);
}

void WindowMediator::update_downloads_dir(const QString &downloads_dir) {
  qDebug() << "::: dl in med" << downloads_dir;
  auto *profile = webview_stack->get_profile();
  profile->setDownloadPath(downloads_dir);
}

WindowMediator::~WindowMediator() {
  disconnect(this);
  delete webview_stack;
}
