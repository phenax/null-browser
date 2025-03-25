#include <QStackedLayout>
#include <QWebEngineHistory>
#include <QWebEngineNewWindowRequest>
#include <QWebEngineProfile>
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
  auto *webview = new WebView(profile);
  webview->setUrl(url);
  layout->addWidget(webview);
  webview_list.append(webview);

  connect(webview->page(), &QWebEnginePage::newWindowRequested, this,
          &WebViewStack::on_new_webview_request);

  if (focus)
    focus_webview(webview_list.length() - 1);

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

void WebViewStack::next() {
  if (webview_list.isEmpty())
    return;
  auto index = current_webview_index() + 1;
  auto total = webview_list.length();
  index = index >= total ? index % total : index;
  focus_webview(index);
}

void WebViewStack::previous() {
  if (webview_list.isEmpty())
    return;
  auto index = current_webview_index() - 1;
  auto total = webview_list.length();
  index = index < 0 ? total + index : index;
  focus_webview(index);
}

void WebViewStack::close_current() { close(current_webview_index()); }

void WebViewStack::close(WebViewId index) {
  if (index < 0 || index >= webview_list.length())
    return;

  auto *webview = webview_list.at(index);
  layout->removeWidget(webview);
  webview_list.removeAt(index);
  disconnect(webview->page());
  webview->deleteLater();

  focus_webview(current_webview_index());

  if (webview_list.isEmpty()) {
    create_new_webview(configuration->new_tab_url, true);
  }
}

void WebViewStack::webview_history_back(WebViewId webview_id,
                                        qsizetype history_index) {
  if (webview_id < 0 || webview_id >= webview_list.length())
    return;

  // TODO: Change this
  auto *webview = webview_list.at(webview_id);
  auto *history = webview->history();
  for (auto i = abs(history_index); i > 0; i--)
    if (history->canGoBack())
      history->back();
}

void WebViewStack::webview_history_forward(WebViewId webview_id,
                                           qsizetype history_index) {
  if (webview_id < 0 || webview_id >= webview_list.length())
    return;

  // TODO: Change this
  auto *webview = webview_list.at(webview_id);
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

uint32_t WebViewStack::current_webview_index() {
  qDebug() << "CIRR" << layout->currentIndex();
  return layout->currentIndex();
}

uint32_t WebViewStack::count() { return webview_list.length(); }

void WebViewStack::focus_webview(WebViewId index) {
  if (webview_list.isEmpty())
    return;

  index = std::max((long long)0,
                   std::min((long long)index, webview_list.length() - 1));
  layout->setCurrentIndex((int)index);
}

QUrl WebViewStack::current_url() {
  if (current_webview_index() >= webview_list.length())
    return QUrl{};

  return webview_list.at(current_webview_index())->url();
}

void WebViewStack::set_current_url(const QUrl &url) {
  if (current_webview_index() >= webview_list.length())
    return;

  webview_list.at(current_webview_index())->setUrl(url);
}
