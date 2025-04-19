#include <QMainWindow>
#include <QWebEngineView>
#include <QtCore>

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
