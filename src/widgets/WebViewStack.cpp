#include <QStackedLayout>
#include <QWebEngineNewWindowRequest>
#include <QWebEngineProfile>
#include <vector>

#include "widgets/WebViewStack.hpp"

WebViewStack::WebViewStack(const Configuration *configuration,
                           QWebEngineProfile *profile, QWidget *parent)
    : QWidget(parent), configuration(configuration), profile(profile) {
  layout = new QStackedLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setStackingMode(QStackedLayout::StackOne);

  create_new_web_view(configuration->new_tab_url, true);
}

void WebViewStack::open_url(const QUrl &url, OpenType open_type) {
  switch (open_type) {
  case OpenType::OpenUrl:
    set_current_url(url);
    break;
  case OpenType::OpenUrlInTab:
    create_new_web_view(url, true);
    break;
  case OpenType::OpenUrlInBgTab:
    create_new_web_view(url, false);
    break;
  case OpenType::OpenUrlInWindow:
    create_new_web_view(url, true);
    break;
  }
}

WebView *WebViewStack::create_new_web_view(const QUrl &url, bool focus) {
  auto *webview = new WebView(profile);
  webview->setUrl(url);
  layout->addWidget(webview);
  web_view_list.append(webview);

  connect(webview->page(), &QWebEnginePage::newWindowRequested, this,
          &WebViewStack::on_new_web_view_request);

  if (focus)
    focus_web_view(web_view_list.length() - 1);

  return webview;
}

QList<Tab> WebViewStack::get_tab_list() {
  QList<Tab> urls;
  for (auto &view : web_view_list)
    urls.append(Tab{.url = view->url().toString(), .title = view->title()});
  return urls;
}

void WebViewStack::on_new_web_view_request(
    const QWebEngineNewWindowRequest &request) {
  switch (request.destination()) {
  case QWebEngineNewWindowRequest::InNewTab:
    create_new_web_view(request.requestedUrl(), true);
    break;
  case QWebEngineNewWindowRequest::InNewBackgroundTab:
    create_new_web_view(request.requestedUrl(), false);
    break;
  case QWebEngineNewWindowRequest::InNewWindow:
  case QWebEngineNewWindowRequest::InNewDialog:
    // TODO: Impl
    create_new_web_view(request.requestedUrl(), true);
    break;
  }
}

void WebViewStack::next() {
  if (web_view_list.isEmpty())
    return;
  auto index = current_web_view_index() + 1;
  auto total = web_view_list.length();
  index = index >= total ? index % total : index;
  focus_web_view(index);
}

void WebViewStack::previous() {
  if (web_view_list.isEmpty())
    return;
  auto index = current_web_view_index() - 1;
  auto total = web_view_list.length();
  index = index < 0 ? total + index : index;
  focus_web_view(index);
}

void WebViewStack::close_current() { close(current_web_view_index()); }

void WebViewStack::close(qsizetype index) {
  if (index < 0 || index >= web_view_list.length())
    return;

  auto *webview = web_view_list.at(index);
  layout->removeWidget(webview);
  web_view_list.removeAt(index);
  disconnect(webview->page());
  webview->deleteLater();

  focus_web_view(current_web_view_index());

  if (web_view_list.isEmpty()) {
    create_new_web_view(configuration->new_tab_url, true);
  }
}

std::vector<QUrl> WebViewStack::urls() {
  std::vector<QUrl> urls;
  for (auto &view : web_view_list)
    urls.push_back(view->url());
  return urls;
}

uint32_t WebViewStack::current_web_view_index() {
  return layout->currentIndex();
}

uint32_t WebViewStack::count() { return web_view_list.length(); }

void WebViewStack::focus_web_view(qsizetype index) {
  if (web_view_list.isEmpty())
    return;

  index = std::max((long long)0,
                   std::min((long long)index, web_view_list.length() - 1));
  layout->setCurrentIndex((int)index);
}

QUrl WebViewStack::current_url() {
  if (current_web_view_index() >= web_view_list.length())
    return QUrl{};

  return web_view_list.at(current_web_view_index())->url();
}

void WebViewStack::set_current_url(const QUrl &url) {
  if (current_web_view_index() >= web_view_list.length())
    return;

  web_view_list.at(current_web_view_index())->setUrl(url);
}
