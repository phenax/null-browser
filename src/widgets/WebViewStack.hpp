#pragma once

#include <QStackedLayout>
#include <QWebEngineProfile>
#include <vector>

#include "Configuration.hpp"
#include "widgets/WebView.hpp"

enum OpenType { OpenUrl, OpenUrlInTab, OpenUrlInBgTab, OpenUrlInWindow };

struct Tab {
  QString url;
  QString title;
};

class WebViewStack : public QWidget {
  Q_OBJECT

public:
  inline static const QUrl NewtabURL = QUrl("about:blank");

public:
  WebViewStack(const Configuration *configuration,
               QWebEngineProfile *profile = new QWebEngineProfile,
               QWidget *parent = nullptr);

  void openUrl(QUrl url, OpenType openType = OpenType::OpenUrl);

  std::vector<QUrl> urls();
  QList<Tab> getTabList();
  u_int32_t currentWebViewIndex();
  u_int32_t count();
  QUrl currentUrl();

  void focusWebView(int32_t index);
  void next();
  void previous();

  void close(int32_t index);
  void closeCurrent();

private:
  void setCurrentUrl(QUrl url);
  WebView *createNewWebView(QUrl url, bool focus = false);

private slots:
  void onNewWebViewRequest(const QWebEngineNewWindowRequest &request);

private:
  const Configuration *configuration;
  QWebEngineProfile *profile;
  QStackedLayout *layout;
  QList<WebView *> webViewList;
};
