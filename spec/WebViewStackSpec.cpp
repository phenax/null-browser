#include <QWebEngineNewWindowRequest>
#include <QWebEngineProfile>
#include <vector>

#include "Configuration.hpp"
#include "testUtils.h"
#include "widgets/WebViewStack.hpp"

class WebViewStackSpec : public QObject {
  Q_OBJECT

  class FakeNewWindowRequest : public QWebEngineNewWindowRequest {
  public:
    FakeNewWindowRequest(DestinationType t, const QRect &r, const QUrl &u,
                         bool b)
        : QWebEngineNewWindowRequest(t, r, u, b, nullptr) {}
  };

private slots:
  void testInitialState() {
    context("when initialized");
    it("opens a single tab") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);

      QCOMPARE(webViewStack.count(), 1);
      QCOMPARE(webViewStack.currentUrl(), configuration.newTabUrl);
      webViewStack.deleteLater();
    }
  }

  void testOpenUrl() {
    context("when openUrl is called without an open type");
    it("replaces current tab url with newtab url") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      webViewStack.openUrl(QUrl("http://a.com"), OpenType::OpenUrl);

      webViewStack.openUrl(QUrl(configuration.newTabUrl));

      QCOMPARE(webViewStack.count(), 1);
      std::vector<QUrl> urls = {QUrl(configuration.newTabUrl)};
      QCOMPARE(webViewStack.urls(), urls);
      QCOMPARE(webViewStack.currentWebViewIndex(), 0);
      QCOMPARE(webViewStack.currentUrl(), configuration.newTabUrl);
    }

    context("when creating a new webview with a url and focus is false");
    it("opens the given url in background") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);

      webViewStack.openUrl(QUrl("https://duckduckgo.com"),
                           OpenType::OpenUrlInBgTab);

      QCOMPARE(webViewStack.count(), 2);
      std::vector<QUrl> urls = {QUrl(configuration.newTabUrl),
                                QUrl("https://duckduckgo.com")};
      QCOMPARE(webViewStack.urls(), urls);
      QCOMPARE(webViewStack.currentWebViewIndex(), 0);
      QCOMPARE(webViewStack.currentUrl(), configuration.newTabUrl);
    }

    context("when creating a new webview with a url and focus is true");
    it("opens the given url as current view") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);

      webViewStack.openUrl(QUrl("https://duckduckgo.com"),
                           OpenType::OpenUrlInTab);

      QCOMPARE(webViewStack.count(), 2);
      std::vector<QUrl> urls = {QUrl(configuration.newTabUrl),
                                QUrl("https://duckduckgo.com")};
      QCOMPARE(webViewStack.urls(), urls);
      QCOMPARE(webViewStack.currentWebViewIndex(), 1);
      QCOMPARE(webViewStack.currentUrl(), QUrl("https://duckduckgo.com"));
    }
  }

  void testNextNavigation() {
    context("when nextWebView is called");
    context("- and there is only 1 tab");
    it("does nothing") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);

      webViewStack.next();

      QCOMPARE(webViewStack.currentWebViewIndex(), 0);
      QCOMPARE(webViewStack.currentUrl(), configuration.newTabUrl);
    }

    context("when nextWebView is called");
    context("- and there are tabs after the current tab");
    it("goes to the next tab") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      webViewStack.openUrl(QUrl("http://a1.com"), OpenType::OpenUrlInBgTab);
      webViewStack.openUrl(QUrl("http://a2.com"), OpenType::OpenUrlInBgTab);

      webViewStack.next();

      QCOMPARE(webViewStack.currentWebViewIndex(), 1);
      QCOMPARE(webViewStack.currentUrl(), QUrl("http://a1.com"));
    }

    context("when nextWebView is called");
    context("- and current tab is the last tab");
    it("jumps to the first tab") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      webViewStack.openUrl(QUrl("http://a1.com"), OpenType::OpenUrlInBgTab);
      webViewStack.openUrl(QUrl("http://a2.com"), OpenType::OpenUrlInTab);
      QCOMPARE(webViewStack.currentWebViewIndex(), 2);

      webViewStack.next();

      QCOMPARE(webViewStack.currentWebViewIndex(), 0);
      QCOMPARE(webViewStack.currentUrl(), configuration.newTabUrl);
    }
  }

  void testPreviousNavigation() {
    context("when previousWebView is called");
    context("- and there is only 1 tab");
    it("does nothing") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);

      webViewStack.previous();

      QCOMPARE(webViewStack.currentWebViewIndex(), 0);
      QCOMPARE(webViewStack.currentUrl(), configuration.newTabUrl);
    }

    context("when previousWebView is called");
    context("- and there are tabs before the current tab");
    it("goes to the next tab") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      webViewStack.openUrl(QUrl("http://a1.com"), OpenType::OpenUrlInBgTab);
      webViewStack.openUrl(QUrl("http://a2.com"), OpenType::OpenUrlInTab);
      QCOMPARE(webViewStack.currentWebViewIndex(), 2);

      webViewStack.previous();

      QCOMPARE(webViewStack.currentWebViewIndex(), 1);
      QCOMPARE(webViewStack.currentUrl(), QUrl("http://a1.com"));
    }

    context("when previousWebView is called");
    context("- and current tab is the last tab");
    it("jumps to the last tab") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      webViewStack.openUrl(QUrl("http://a1.com"), OpenType::OpenUrlInBgTab);
      webViewStack.openUrl(QUrl("http://a2.com"), OpenType::OpenUrlInBgTab);

      webViewStack.previous();

      QCOMPARE(webViewStack.currentWebViewIndex(), 2);
      QCOMPARE(webViewStack.currentUrl(), QUrl("http://a2.com"));
    }
  }

  void testCloseWebView() {
    context("when closeWebView is called");
    context("- with out of bounds index");
    it("does nothing") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      webViewStack.openUrl(QUrl("https://a.com"), OpenType::OpenUrl);

      webViewStack.close(1);

      QCOMPARE(webViewStack.count(), 1);
      QCOMPARE(webViewStack.urls(), (std::vector<QUrl>{QUrl("https://a.com")}));
      QCOMPARE(webViewStack.currentWebViewIndex(), 0);
      QCOMPARE(webViewStack.currentUrl(), QUrl("https://a.com"));
    }

    context("when closeWebView is called");
    context("- and there is only 1 tab");
    it("closes the tab and opens empty tab in its place") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      webViewStack.openUrl(QUrl("https://a.com"), OpenType::OpenUrl);
      QCOMPARE(webViewStack.count(), 1);

      webViewStack.close(0);

      QCOMPARE(webViewStack.count(), 1);
      QCOMPARE(webViewStack.urls(),
               (std::vector<QUrl>{configuration.newTabUrl}));
      QCOMPARE(webViewStack.currentWebViewIndex(), 0);
      QCOMPARE(webViewStack.currentUrl(), configuration.newTabUrl);
    }

    context("when closeWebView is called");
    context("- with the current tab index");
    context("- and there are some tabs after");
    it("closes the tab and focuses the next tab") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      webViewStack.openUrl(QUrl("https://a1.com"), OpenType::OpenUrlInTab);
      webViewStack.openUrl(QUrl("https://a2.com"), OpenType::OpenUrlInBgTab);
      QCOMPARE(webViewStack.count(), 3);
      QCOMPARE(webViewStack.currentWebViewIndex(), 1);

      webViewStack.close(1);

      QCOMPARE(webViewStack.count(), 2);
      QCOMPARE(
          webViewStack.urls(),
          (std::vector<QUrl>{configuration.newTabUrl, QUrl("https://a2.com")}));
      QCOMPARE(webViewStack.currentWebViewIndex(), 1);
      QCOMPARE(webViewStack.currentUrl(), QUrl("https://a2.com"));
    }

    context("when closeWebView is called");
    context("- with the current tab index");
    context("- which is the last tab");
    it("closes the tab and focusses previous tab") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      webViewStack.openUrl(QUrl("https://a1.com"), OpenType::OpenUrlInBgTab);
      webViewStack.openUrl(QUrl("https://a2.com"), OpenType::OpenUrlInTab);
      QCOMPARE(webViewStack.count(), 3);
      QCOMPARE(webViewStack.currentWebViewIndex(), 2);

      webViewStack.close(2);

      QCOMPARE(webViewStack.count(), 2);
      QCOMPARE(
          webViewStack.urls(),
          (std::vector<QUrl>{configuration.newTabUrl, QUrl("https://a1.com")}));
      QCOMPARE(webViewStack.currentWebViewIndex(), 1);
      QCOMPARE(webViewStack.currentUrl(), QUrl("https://a1.com"));
    }
  }

  void testNewWindowRequestSignal() {
    context("when webview emits a newWindowRequested signal");
    context("- of type new tab");
    it("opens a new web view and focusses it") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      webViewStack.openUrl(QUrl("https://a.com"), OpenType::OpenUrl);
      auto webview = webViewStack.findChild<QWebEngineView *>();
      QCOMPARE(webViewStack.count(), 1);

      FakeNewWindowRequest windowRequest(
          FakeNewWindowRequest::DestinationType::InNewTab, QRect(0, 0, 0, 0),
          QUrl("https://new.com"), true);
      emit webview->page()->newWindowRequested(windowRequest);

      QCOMPARE(webViewStack.count(), 2);
      QCOMPARE(
          webViewStack.urls(),
          (std::vector<QUrl>{QUrl("https://a.com"), QUrl("https://new.com")}));
      QCOMPARE(webViewStack.currentWebViewIndex(), 1);
      QCOMPARE(webViewStack.currentUrl(), QUrl("https://new.com"));
    }

    context("when webview emits a newWindowRequested signal");
    context("- of type new background tab");
    it("opens a new web view in the background") {
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      webViewStack.openUrl(QUrl("https://a.com"), OpenType::OpenUrl);
      auto webview = webViewStack.findChild<QWebEngineView *>();

      FakeNewWindowRequest windowRequest(
          FakeNewWindowRequest::DestinationType::InNewBackgroundTab,
          QRect(0, 0, 0, 0), QUrl("https://new.com"), true);
      emit webview->page()->newWindowRequested(windowRequest);

      QCOMPARE(webViewStack.count(), 2);
      QCOMPARE(
          webViewStack.urls(),
          (std::vector<QUrl>{QUrl("https://a.com"), QUrl("https://new.com")}));
      QCOMPARE(webViewStack.currentWebViewIndex(), 0);
      QCOMPARE(webViewStack.currentUrl(), QUrl("https://a.com"));
    }
  }
};

QTEST_REGISTER(WebViewStackSpec)
#include "WebViewStackSpec.moc"
