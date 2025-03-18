#pragma once

#include <QStackedLayout>
#include <QWebEngineProfile>
#include <sys/types.h>

#include "Configuration.hpp"
#include "completion/TabsModel.hpp"
#include "widgets/WebView.hpp"

enum OpenType { OpenUrl, OpenUrlInTab, OpenUrlInBgTab, OpenUrlInWindow };

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

  void focusWebView(long index);
  void next();
  void previous();

  void close(long index);
  void closeCurrent();

private:
  void setCurrentUrl(QUrl url);
  WebView *createNewWebView(QUrl url, bool focus = false);

private slots:
  void onNewWebViewRequest(QWebEngineNewWindowRequest &request);

private:
  const Configuration *configuration;
  QWebEngineProfile *profile;
  QStackedLayout *layout;
  QList<WebView *> webViewList;
};
