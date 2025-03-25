#pragma once

#include <QStackedLayout>
#include <QWebEngineProfile>
#include <cstdint>
#include <vector>

#include "Configuration.hpp"
#include "widgets/WebView.hpp"

using WebViewId = qsizetype;

enum OpenType : uint8_t {
  OpenUrl,
  OpenUrlInTab,
  OpenUrlInBgTab,
  OpenUrlInWindow
};

struct Tab {
  QString url;
  QString title;
};

class WebViewStack : public QWidget {
  Q_OBJECT

public:
  WebViewStack(const Configuration *configuration,
               QWebEngineProfile *profile = new QWebEngineProfile,
               QWidget *parent = nullptr);

  void open_url(const QUrl &url, OpenType open_type = OpenType::OpenUrl);

  std::vector<QUrl> urls();
  QList<Tab> get_webview_list();
  uint32_t current_webview_index();
  uint32_t count();
  QUrl current_url();

  void focus_webview(WebViewId index);
  void next();
  void previous();

  void close(WebViewId index);
  void close_current();

  void webview_history_back(WebViewId webview_id, qsizetype history_index);
  void webview_history_forward(WebViewId webview_id, qsizetype history_index);

private slots:
  void on_new_webview_request(const QWebEngineNewWindowRequest &request);

protected:
  void set_current_url(const QUrl &url);
  WebView *create_new_webview(const QUrl &url, bool focus = false);

private:
  const Configuration *configuration;
  QWebEngineProfile *profile;
  QStackedLayout *layout;
  QList<WebView *> webview_list;
};
