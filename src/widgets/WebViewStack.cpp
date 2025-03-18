#include <QStackedLayout>
#include <QWebEngineNewWindowRequest>

#include "widgets/WebViewStack.hpp"

WebViewStack::WebViewStack(const Configuration *configuration,
                           QWebEngineProfile *profile, QWidget *parent)
    : QWidget(parent), profile(profile), configuration(configuration) {
  layout = new QStackedLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setStackingMode(QStackedLayout::StackOne);

  createNewWebView(configuration->newTabUrl, true);
}

void WebViewStack::openUrl(QUrl url, OpenType openType) {
  switch (openType) {
  case OpenType::OpenUrl:
    setCurrentUrl(url);
    break;
  case OpenType::OpenUrlInTab:
    createNewWebView(url, true);
    break;
  case OpenType::OpenUrlInBgTab:
    createNewWebView(url, false);
    break;
  case OpenType::OpenUrlInWindow:
    createNewWebView(url, true);
    break;
  }
}

WebView *WebViewStack::createNewWebView(QUrl url, bool focus) {
  auto webview = new WebView(profile);
  webview->setUrl(url);
  layout->addWidget(webview);
  webViewList.append(webview);

  connect(webview->page(), &QWebEnginePage::newWindowRequested, this,
          &WebViewStack::onNewWebViewRequest);

  if (focus)
    focusWebView(webViewList.length() - 1);

  return webview;
}

QList<Tab> WebViewStack::getTabList() {
  QList<Tab> urls;
  for (auto &view : webViewList)
    urls.append((Tab){.url = view->url().toString(), .title = view->title()});
  return urls;
}

void WebViewStack::onNewWebViewRequest(QWebEngineNewWindowRequest &request) {
  switch (request.destination()) {
  case QWebEngineNewWindowRequest::InNewTab:
    createNewWebView(request.requestedUrl(), true);
    break;
  case QWebEngineNewWindowRequest::InNewBackgroundTab:
    createNewWebView(request.requestedUrl(), false);
    break;
  case QWebEngineNewWindowRequest::InNewWindow:
    // TODO: Impl
    createNewWebView(request.requestedUrl(), true);
    break;
  case QWebEngineNewWindowRequest::InNewDialog:
    // TODO: Impl
    createNewWebView(request.requestedUrl(), true);
    break;
  }
}

void WebViewStack::next() {
  if (webViewList.isEmpty())
    return;
  auto index = currentWebViewIndex() + 1;
  auto total = webViewList.length();
  index = index >= total ? index % total : index;
  focusWebView(index);
}

void WebViewStack::previous() {
  if (webViewList.isEmpty())
    return;
  auto index = currentWebViewIndex() - 1;
  auto total = webViewList.length();
  index = index < 0 ? total + index : index;
  focusWebView(index);
}

void WebViewStack::closeCurrent() { close(currentWebViewIndex()); }

void WebViewStack::close(long index) {
  if (index < 0 || index >= webViewList.length())
    return;

  auto webview = webViewList.at(index);
  layout->removeWidget(webview);
  webViewList.removeAt(index);
  disconnect(webview->page());
  webview->deleteLater();

  focusWebView(currentWebViewIndex());

  if (webViewList.isEmpty()) {
    createNewWebView(configuration->newTabUrl, true);
  }
}

std::vector<QUrl> WebViewStack::urls() {
  std::vector<QUrl> urls;
  for (auto &view : webViewList)
    urls.push_back(view->url());
  return urls;
}

u_int32_t WebViewStack::currentWebViewIndex() { return layout->currentIndex(); }

u_int32_t WebViewStack::count() { return webViewList.length(); }

void WebViewStack::focusWebView(long index) {
  if (webViewList.isEmpty())
    return;

  index = std::max((long)0, std::min(index, (long)webViewList.length() - 1));
  layout->setCurrentIndex(index);
}

QUrl WebViewStack::currentUrl() {
  if (currentWebViewIndex() >= webViewList.length())
    return QUrl("");

  return webViewList.at(currentWebViewIndex())->url();
}

void WebViewStack::setCurrentUrl(QUrl url) {
  if (currentWebViewIndex() >= webViewList.length())
    return;

  webViewList.at(currentWebViewIndex())->setUrl(url);
}
