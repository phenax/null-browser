#include <QStackedLayout>
#include <QWebEngineHistory>
#include <QWebEngineNewWindowRequest>
#include <QWebEngineProfile>
#include <cstdint>
#include <cstdlib>
#include <vector>

#include "widgets/WebViewStack.hpp"

WebViewStack::WebViewStack(const Configuration *configuration,
                           QWebEngineProfile *profile, QWidget *parent)
    : QWidget(parent), configuration(configuration), profile(profile) {
  layout = new QStackedLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setStackingMode(QStackedLayout::StackOne);

  create_new_webview(configuration->new_tab_url, true);
}

void WebViewStack::open_url(const QUrl &url, OpenType open_type) {
  switch (open_type) {
  case OpenType::OpenUrl:
    set_current_url(url);
    break;
  case OpenType::OpenUrlInTab:
    create_new_webview(url, true);
    break;
  case OpenType::OpenUrlInBgTab:
    create_new_webview(url, false);
    break;
  case OpenType::OpenUrlInWindow:
    create_new_webview(url, true);
    break;
  }
}

WebView *WebViewStack::create_new_webview(const QUrl &url, bool focus) {
  auto *webview = new WebView(next_id++, profile);
  webview->setUrl(url);
  layout->addWidget(webview);
  webview_list.append(webview);

  connect(webview->page(), &QWebEnginePage::newWindowRequested, this,
          &WebViewStack::on_new_webview_request);

  if (focus)
    focus_webview(webview->get_id());

  return webview;
}

QList<Tab> WebViewStack::get_webview_list() {
  QList<Tab> urls;
  for (auto &view : webview_list)
    urls.append(Tab{.url = view->url().toString(), .title = view->title()});
  return urls;
}

void WebViewStack::on_new_webview_request(
    const QWebEngineNewWindowRequest &request) {
  switch (request.destination()) {
  case QWebEngineNewWindowRequest::InNewTab:
    create_new_webview(request.requestedUrl(), true);
    break;
  case QWebEngineNewWindowRequest::InNewBackgroundTab:
    create_new_webview(request.requestedUrl(), false);
    break;
  case QWebEngineNewWindowRequest::InNewWindow:
  case QWebEngineNewWindowRequest::InNewDialog:
    // TODO: Impl
    create_new_webview(request.requestedUrl(), true);
    break;
  }
}

int32_t WebViewStack::get_webview_index(WebViewId webview_id) {
  int index = 0;
  for (auto &webview : webview_list) {
    if (webview->get_id() == webview_id)
      return index;
    index++;
  }
  return -1;
}

void WebViewStack::close(WebViewId webview_id) {
  auto *webview = get_webview(webview_id);
  if (webview == nullptr)
    return;

  auto webview_index = get_webview_index(webview_id);
  if (webview_index < 0)
    return;

  layout->removeWidget(webview);
  webview_list.removeAt(webview_index);
  disconnect(webview->page());
  webview->deleteLater();

  // TODO: Focus on different webview
  // focus_webview();

  if (webview_list.isEmpty()) {
    create_new_webview(configuration->new_tab_url, true);
  }
}

void WebViewStack::webview_history_back(WebViewId webview_id,
                                        qsizetype history_index) {
  auto *webview = get_webview(webview_id);
  if (webview == nullptr) {
    qDebug() << "Invalid webview id" << webview_id;
    return;
  }

  // TODO: Change this
  auto *history = webview->history();
  for (auto i = abs(history_index); i > 0; i--)
    if (history->canGoBack())
      history->back();
}

void WebViewStack::webview_history_forward(WebViewId webview_id,
                                           qsizetype history_index) {
  auto *webview = get_webview(webview_id);
  if (webview == nullptr) {
    qDebug() << "Invalid webview id" << webview_id;
    return;
  }

  // TODO: Change this
  auto *history = webview->history();
  for (auto i = abs(history_index); i > 0; i--)
    if (history->canGoForward())
      history->forward();
}

std::vector<QUrl> WebViewStack::urls() {
  std::vector<QUrl> urls;
  for (auto &view : webview_list)
    urls.push_back(view->url());
  return urls;
}

WebViewId WebViewStack::current_webview_id() {
  if (webview_list.empty())
    return -1;
  return current_webview()->get_id();
}

WebView *WebViewStack::current_webview() {
  if (webview_list.empty())
    return nullptr;
  return webview_list.at(current_webview_index());
}

uint32_t WebViewStack::current_webview_index() {
  return layout->currentIndex();
}

uint32_t WebViewStack::count() { return webview_list.length(); }

void WebViewStack::focus_webview(WebViewId webview_id) {
  auto webview_index = get_webview_index(webview_id);
  if (webview_index >= 0)
    layout->setCurrentIndex((int)webview_index);
}

WebView *WebViewStack::get_webview(WebViewId webview_id) {
  auto webview_index = get_webview_index(webview_id);
  if (webview_index < 0)
    return nullptr;
  return webview_list.at(webview_index);
}

QUrl WebViewStack::current_url() {
  auto *webview = current_webview();
  if (webview == nullptr) {
    qDebug() << "No current webview";
    return QUrl{};
  }

  return webview->url();
}

void WebViewStack::set_current_url(const QUrl &url) {
  auto *webview = current_webview();
  if (webview == nullptr) {
    qDebug() << "No current webview";
    return;
  }

  webview->setUrl(url);
}
