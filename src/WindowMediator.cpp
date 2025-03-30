#include <QList>
#include <QWidget>
#include <QtCore>

#include "WindowMediator.hpp"
#include "widgets/WebViewStack.hpp"

WindowMediator::WindowMediator(WebViewStack *webview_stack)
    : webview_stack(webview_stack) {
  connect(this, &WindowMediator::history_back_requested, webview_stack,
          &WebViewStack::webview_history_back);
  connect(this, &WindowMediator::history_forward_requested, webview_stack,
          &WebViewStack::webview_history_forward);

  connect(this, &WindowMediator::url_opened, webview_stack,
          &WebViewStack::open_url);
  connect(this, &WindowMediator::webview_closed, webview_stack,
          &WebViewStack::close);
  connect(this, &WindowMediator::webview_selected, webview_stack,
          &WebViewStack::focus_webview);
}

WindowMediator::~WindowMediator() {
  disconnect(this);
  delete webview_stack;
}
