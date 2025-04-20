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

void WebView::scroll_increment(int deltax, int deltay) {
  auto code = QString(R"((() => {
    const $el = document.scrollingElement;
    $el.scrollTo($el.scrollLeft + %1, $el.scrollTop + %2);
  })())")
                  .arg(deltax)
                  .arg(deltay);

  page()->runJavaScript(code);
}

void WebView::scroll_to_top() {
  auto code = QString(R"(document.scrollingElement.scrollTo(0, 0))");
  page()->runJavaScript(code);
}

void WebView::scroll_to_bottom() {
  auto code =
      QString(R"(document.scrollingElement.scrollTo(0, document.scrollingElement.scrollHeight))");
  page()->runJavaScript(code);
}
