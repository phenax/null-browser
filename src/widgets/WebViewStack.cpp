#include <QStackedLayout>
#include <QWebEngineFindTextResult>
#include <QWebEngineHistory>
#include <QWebEngineNewWindowRequest>
#include <QWebEngineProfile>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <qwebengineprofile.h>
#include <vector>

#include "WindowActionRouter.hpp"
#include "events/PermissionRequestedEvent.hpp"
#include "events/UrlChangedEvent.hpp"

#include "widgets/WebViewStack.hpp"

WebViewId WebViewStack::next_webview_id = 1;

WebViewStack::WebViewStack(const Configuration *configuration, QWebEngineProfile *profile,
                           QWidget *parent)
    : QWidget(parent), configuration(configuration), profile(profile) {
  layout = new QStackedLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setStackingMode(QStackedLayout::StackOne);

  connect(layout, &QStackedLayout::currentChanged, this,
          &WebViewStack::current_webview_title_changed);
  connect(profile, &QWebEngineProfile::downloadRequested, this, &WebViewStack::on_download_request);
}

void WebViewStack::open_url(const QUrl &url, OpenType open_type, WebViewId webview_id) {
  switch (open_type) {
  case OpenType::OpenUrl:
    set_webview_url(url, webview_id);
    break;
  case OpenType::OpenUrlInView:
    create_new_webview(url, true);
    break;
  case OpenType::OpenUrlInBgView:
    create_new_webview(url, false);
    break;
  case OpenType::OpenUrlInWindow:
    emit new_window_requested(url);
    break;
  }
}

WebView *WebViewStack::create_new_webview(const QUrl &url, bool focus) {
  auto *webview = new WebView(WebViewStack::next_webview_id++, profile);
  webview->setUrl(url);
  layout->addWidget(webview);
  webview_list.append(webview);

  auto *page = webview->page();
  connect(page, &QWebEnginePage::newWindowRequested, this, &WebViewStack::on_new_webview_request);
  connect(page, &QWebEnginePage::urlChanged, this, [webview](const QUrl &url) {
    // TODO: Add window id
    auto *event = new UrlChangedEvent(url.toString(), webview->get_id(), 0);
    WindowActionRouter::instance().dispatch_event(event);
  });
  connect(page, &QWebEnginePage::titleChanged, this, [this](const QString & /* title */) {
    emit current_webview_title_changed(layout->currentIndex());
  });
  connect(page, &QWebEnginePage::permissionRequested, this, [webview](QWebEnginePermission perm) {
    auto permission = std::make_shared<QWebEnginePermission>(std::move(perm));
    // TODO: Add windown id
    auto *event = PermissionRequestedEvent::from_permission(permission, webview->get_id(), 0);
    WindowActionRouter::instance().dispatch_event(event);
  });

  if (focus)
    focus_webview(webview->get_id());

  emit current_webview_title_changed(layout->currentIndex());

  return webview;
}

QList<WebViewData> WebViewStack::get_webview_list() {
  QList<WebViewData> urls;
  for (auto &view : webview_list) {
    urls.append(WebViewData{
        .id = view->get_id(),
        .url = view->url().toString(),
        .title = view->title(),
    });
  }
  return urls;
}

void WebViewStack::on_new_webview_request(QWebEngineNewWindowRequest &request) {
  switch (request.destination()) {
  case QWebEngineNewWindowRequest::InNewTab:
    open_url(request.requestedUrl(), OpenType::OpenUrlInView);
    break;
  case QWebEngineNewWindowRequest::InNewBackgroundTab:
    open_url(request.requestedUrl(), OpenType::OpenUrlInBgView);
    break;
  case QWebEngineNewWindowRequest::InNewWindow:
  case QWebEngineNewWindowRequest::InNewDialog:
    // NOTE: Using tabs for now instead of windows
    auto *webview = create_new_webview(request.requestedUrl(), true);
    request.openIn(webview->page());
    // emit new_window_requested(request.requestedUrl());
    break;
  }
}

void WebViewStack::on_download_request(QWebEngineDownloadRequest *download) {
  qDebug() << "DOWNLOADING" << download->downloadDirectory() << download->downloadFileName();
  emit download->accept();
  // TODO: Emit event to see download progress
}

int32_t WebViewStack::get_webview_index(WebViewId webview_id) {
  if (webview_id == 0) {
    // TODO: Check how to detect "active" window before qapp start (lua init)?
    if (window()->isActiveWindow())
      webview_id = current_webview_id();
    else
      return -1;
  }

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

  if (webview_list.isEmpty()) {
    if (configuration->close_window_when_no_views()) {
      emit close_window_requested();
    } else {
      create_new_webview(configuration->new_view_url(), true);
    }
  }
}

void WebViewStack::webview_history_back(WebViewId webview_id, qsizetype history_index) {
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

void WebViewStack::webview_history_forward(WebViewId webview_id, qsizetype history_index) {
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

uint32_t WebViewStack::current_webview_index() { return layout->currentIndex(); }

uint32_t WebViewStack::count() { return webview_list.length(); }

void WebViewStack::focus_webview(WebViewId webview_id) {
  auto webview_index = get_webview_index(webview_id);
  if (webview_index >= 0)
    layout->setCurrentIndex((int)webview_index);
}

void WebViewStack::open_devtools(WebViewId webview_id) {
  auto *webview = get_webview(webview_id);
  if (webview == nullptr) {
    qDebug() << "Webview does not exist";
    return;
  }

  webview->open_devtools();
}

void WebViewStack::set_search_text(const QString &text, WebViewId webview_id, bool forward) {
  auto *webview = get_webview(webview_id);
  if (webview == nullptr) {
    qDebug() << "Webview does not exist";
    return;
  }

  QString search_text = text.trimmed();

  auto flags =
      search_text.isLower() ? QWebEnginePage::FindFlags() : QWebEnginePage::FindCaseSensitively;
  if (!forward)
    flags |= QWebEnginePage::FindBackward;

  webview->page()->findText(search_text, flags, [](const QWebEngineFindTextResult &result) {
    qDebug() << result.numberOfMatches() << result.activeMatch();
  });
}

WebView *WebViewStack::get_webview(WebViewId webview_id) {
  auto webview_index = get_webview_index(webview_id);
  if (webview_index < 0)
    return nullptr;
  return webview_list.at(webview_index);
}

bool WebViewStack::has_webview(WebViewId webview_id) { return get_webview(webview_id) != nullptr; }

QUrl WebViewStack::current_url() {
  auto *webview = current_webview();
  if (webview == nullptr) {
    qDebug() << "No current webview";
    return QUrl{configuration->new_view_url()};
  }

  return webview->url();
}

void WebViewStack::set_current_url(const QUrl &url) {
  if (count() == 0) {
    create_new_webview(url, true);
    return;
  }

  auto *webview = current_webview();
  if (webview == nullptr) {
    qDebug() << "No current webview";
    return;
  }

  webview->setUrl(url);
}

void WebViewStack::set_webview_url(const QUrl &url, WebViewId webview_id) {
  if (webview_id == 0) {
    set_current_url(url);
    return;
  }

  auto *webview = get_webview(webview_id);
  if (webview == nullptr) {
    qDebug() << "Webview does not exist";
    return;
  }

  webview->setUrl(url);
}

void WebViewStack::scroll(WebViewId webview_id, int deltax, int deltay) {
  auto *webview = get_webview(webview_id);
  if (webview == nullptr) {
    qDebug() << "Webview does not exist";
    return;
  }

  webview->scroll_increment(deltax, deltay);
}

void WebViewStack::scroll_to_top(WebViewId webview_id) {
  auto *webview = get_webview(webview_id);
  if (webview == nullptr) {
    qDebug() << "Webview does not exist";
    return;
  }

  webview->scroll_to_top();
}

void WebViewStack::scroll_to_bottom(WebViewId webview_id) {
  auto *webview = get_webview(webview_id);
  if (webview == nullptr) {
    qDebug() << "Webview does not exist";
    return;
  }

  webview->scroll_to_bottom();
}

void WebViewStack::set_html(const QString &html, WebViewId webview_id) {
  auto *webview = get_webview(webview_id);
  if (webview == nullptr) {
    qDebug() << "Webview does not exist";
    return;
  }

  webview->setHtml(html);
}

void WebViewStack::expose_rpc_function(const QString &name, const RpcFunc & /* unused */,
                                       WebViewId /* unused */) {
  qDebug() << "expose_rpc_function: NOT IMPLEMENTED" << name;
}
