#pragma once

#include <QStackedLayout>
#include <QWebEngineProfile>
#include <QWebEngineView>
#include <sys/types.h>

#include "completion/TabsModel.hpp"

enum OpenType { OpenUrl, OpenUrlInTab, OpenUrlInBgTab, OpenUrlInWindow };

class WebViewStack : public QWidget {
  Q_OBJECT

public:
  inline static const QUrl NewtabURL = QUrl("about:blank");

public:
  WebViewStack(QWebEngineProfile *profile = new QWebEngineProfile,
               QWidget *parent = nullptr);

  void openUrl(QUrl url = WebViewStack::NewtabURL,
               OpenType openType = OpenType::OpenUrl);

  std::vector<QUrl> urls();
  u_int32_t currentWebViewIndex();
  u_int32_t count();
  QUrl currentUrl();

  void focusWebView(long index);
  void next();
  void previous();

  void close(long index);
  void closeCurrent();

  QList<Tab> getTabList();

private:
  void setCurrentUrl(QUrl url);
  QWebEngineView *createNewWebView(QUrl url = WebViewStack::NewtabURL,
                                   bool focus = false);

private slots:
  void onNewWebViewRequest(QWebEngineNewWindowRequest &request);

private:
  QWebEngineProfile *profile;
  QStackedLayout *layout;
  QList<QWebEngineView *> webViewList;
};
