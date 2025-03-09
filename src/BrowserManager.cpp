#include <QStackedLayout>
#include <QWebEngineNewWindowRequest>
#include <QWebEngineView>

#include "BrowserManager.hpp"

BrowserManager::BrowserManager(QWebEngineProfile *profile) : QWidget() {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout = new QStackedLayout(this);
  layout->setStackingMode(QStackedLayout::StackAll);
  layout->setContentsMargins(0, 0, 0, 0);

  this->profile = profile;

  createNewWebView(BrowserManager::NewtabURL, true);
}

QWebEngineView *BrowserManager::createNewWebView(QUrl url, bool focus) {
  auto webview = new QWebEngineView(profile);
  webview->setUrl(url);
  layout->addWidget(webview);
  webViewList.append(webview);

  connect(webview->page(), &QWebEnginePage::newWindowRequested, this,
          &BrowserManager::onNewWebViewRequest);

  if (focus)
    focusWebView(webViewList.length() - 1);

  return webview;
}

void BrowserManager::onNewWebViewRequest(QWebEngineNewWindowRequest &request) {
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

void BrowserManager::nextWebView() {
  if (webViewList.isEmpty())
    return;
  auto index = currentWebViewIndex() + 1;
  auto total = webViewList.length();
  index = index >= total ? index % total : index;
  focusWebView(index);
}

void BrowserManager::previousWebView() {
  if (webViewList.isEmpty())
    return;
  auto index = currentWebViewIndex() - 1;
  auto total = webViewList.length();
  index = index < 0 ? total + index : index;
  focusWebView(index);
}

void BrowserManager::closeCurrentWebView() {
  closeWebView(currentWebViewIndex());
}

void BrowserManager::closeWebView(long index) {
  if (index < 0 || index >= webViewList.length())
    return;

  auto webview = webViewList.at(index);
  layout->removeWidget(webview);
  webViewList.removeAt(index);
  disconnect(webview->page());
  webview->deleteLater();

  focusWebView(currentWebViewIndex());

  if (webViewList.isEmpty()) {
    createNewWebView(BrowserManager::NewtabURL, true);
  }
}

std::vector<QUrl> BrowserManager::webViewUrls() {
  std::vector<QUrl> urls;
  for (auto &view : webViewList)
    urls.push_back(view->url());
  return urls;
}

u_int32_t BrowserManager::currentWebViewIndex() {
  return layout->currentIndex();
}
u_int32_t BrowserManager::webViewCount() { return webViewList.length(); }

void BrowserManager::focusWebView(long index) {
  if (webViewList.isEmpty())
    return;

  index = std::max((long)0, std::min(index, (long)webViewList.length() - 1));
  layout->setCurrentIndex(index);
}

QUrl BrowserManager::currentUrl() {
  if (currentWebViewIndex() >= webViewList.length())
    return QUrl("about:newtab");

  return webViewList.at(currentWebViewIndex())->url();
}

void BrowserManager::setCurrentUrl(QUrl url) {
  if (currentWebViewIndex() >= webViewList.length())
    return;

  webViewList.at(currentWebViewIndex())->setUrl(url);
}
