#pragma once

#include <QStackedLayout>
#include <QWebEngineProfile>
#include <QWebEngineView>
#include <sys/types.h>

enum OpenType { OpenUrl, OpenUrlInTab, OpenUrlInBgTab, OpenUrlInWindow };

class BrowserManager : public QWidget {
  Q_OBJECT

public:
  inline static const QUrl NewtabURL = QUrl("about:blank");

public:
  BrowserManager(QWebEngineProfile *profile = new QWebEngineProfile);

  QUrl currentUrl();
  void setCurrentUrl(QUrl url);

  QWebEngineView *createNewWebView(QUrl url = BrowserManager::NewtabURL,
                                   bool focus = false);

  void openUrl(QUrl url = BrowserManager::NewtabURL,
               OpenType openType = OpenType::OpenUrl);

  std::vector<QUrl> webViewUrls();
  u_int32_t currentWebViewIndex();
  u_int32_t webViewCount();
  void focusWebView(long index);
  void nextWebView();
  void previousWebView();

  void closeWebView(long index);
  void closeCurrentWebView();

  void onNewWebViewRequest(QWebEngineNewWindowRequest &request);

private:
  QWebEngineProfile *profile;
  QStackedLayout *layout;
  QList<QWebEngineView *> webViewList;
};
