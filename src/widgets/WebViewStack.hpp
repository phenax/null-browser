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

struct WebViewData {
  WebViewId id;
  QString url;
  QString title;
};

class WebViewStack : public QWidget {
  Q_OBJECT

public:
  WebViewStack(const Configuration *configuration,
               QWebEngineProfile *profile = new QWebEngineProfile,
               QWidget *parent = nullptr);

  QList<WebViewData> get_webview_list();
  WebView *current_webview();
  WebViewId current_webview_id();
  uint32_t count();
  QUrl current_url();
  void set_webview_url(const QUrl &url, WebViewId webview_id);

  bool has_webview(WebViewId webview_id);

  /// @deprecated TODO: Remove
  std::vector<QUrl> urls();
  /// @deprecated TODO: Remove
  uint32_t current_webview_index();

signals:
  void current_webview_changed(int index);

protected:
  void set_current_url(const QUrl &url);
  WebView *create_new_webview(const QUrl &url, bool focus = false);
  int32_t get_webview_index(WebViewId webview_id);
  WebView *get_webview(WebViewId webview_id);

public slots:
  void open_url(const QUrl &url, OpenType open_type = OpenType::OpenUrl,
                WebViewId webview_id = 0);
  void webview_history_back(WebViewId webview_id, qsizetype history_index);
  void webview_history_forward(WebViewId webview_id, qsizetype history_index);
  void close(WebViewId webview_id);
  void focus_webview(WebViewId webview_id);

protected slots:
  void on_new_webview_request(const QWebEngineNewWindowRequest &request);

private:
  const Configuration *configuration;
  QWebEngineProfile *profile;
  QStackedLayout *layout;
  QList<WebView *> webview_list;
};
