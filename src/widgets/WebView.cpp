#include <QMainWindow>
#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineView>
#include <QtCore>
#include <qwebenginepage.h>
#include <qwebengineprofile.h>

#include "schemes/NullRpcSchemeHandler.hpp"

#include "widgets/WebView.hpp"

WebView::WebView(uint32_t webview_id, QWebEngineProfile *profile, QWidget *parent_node)
    : QWebEngineView(profile, parent_node), id(webview_id) {}

void WebView::open_devtools() {
  if (devtools_window != nullptr)
    return;

  devtools_window = new DevtoolsWindow(page()->profile());
  devtools_window->show();

  connect(devtools_window, &DevtoolsWindow::closed, this, [this]() {
    devtools_window->deleteLater();
    devtools_window = nullptr;
  });

  page()->setDevToolsPage(devtools_window->page());
}

void WebView::scroll_increment(int deltax, int deltay) {
  auto code = QString(R"JS((() => {
    const $el = document.scrollingElement;
    $el.scrollTo($el.scrollLeft + %1, $el.scrollTop + %2);
  })())JS")
                  .arg(deltax)
                  .arg(deltay);

  page()->runJavaScript(code);
}

void WebView::scroll_to_top() {
  auto code = QString(R"JS(document.scrollingElement.scrollTo(0, 0))JS");
  page()->runJavaScript(code);
}

void WebView::scroll_to_bottom() {
  auto code = QString(
      R"JS(document.scrollingElement.scrollTo(0, document.scrollingElement.scrollHeight))JS");
  page()->runJavaScript(code);
}

void WebView::enable_rpc_api() {
  rpc_enabled = true;
  auto &nullrpc = NullRPCSchemeHandler::instance();
  connect(&nullrpc, &NullRPCSchemeHandler::message_received, this, &WebView::on_rpc_message);
}

void WebView::expose_rpc_function(const QString &name, const RpcFunc &action) {
  exposed_functions.insert({name, action});
}

void WebView::on_rpc_message(const NullRPCMessage &message) {
  if (!rpc_enabled || !exposed_functions.contains(message.name))
    return;

  RpcArgs args;
  for (auto pair : message.params.queryItems())
    args.insert(pair);

  auto func = exposed_functions.at(message.name);
  func(args);
}
