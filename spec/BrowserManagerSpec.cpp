#include <QWebEngineNewWindowRequest>
#include <QWebEngineProfile>
#include <vector>

#include "testUtils.h"
#include "widgets/BrowserManager.hpp"

class BrowserManagerSpec : public QObject {
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
      BrowserManager browserManager;

      QCOMPARE(browserManager.webViewCount(), 1);
      QCOMPARE(browserManager.currentUrl(), BrowserManager::NewtabURL);
      browserManager.deleteLater();
    }
  }

  void testCreateNewWebView() {
    context("when creating a new background webview without a url");
    it("opens new tab url as a background view") {
      BrowserManager browserManager;

      browserManager.createNewWebView();

      QCOMPARE(browserManager.webViewCount(), 2);
      std::vector<QUrl> urls = {QUrl(BrowserManager::NewtabURL),
                                QUrl(BrowserManager::NewtabURL)};
      QCOMPARE(browserManager.webViewUrls(), urls);
      QCOMPARE(browserManager.currentWebViewIndex(), 0);
      QCOMPARE(browserManager.currentUrl(), BrowserManager::NewtabURL);
    }

    context("when creating a new webview with a url and focus is false");
    it("opens the given url in background") {
      BrowserManager browserManager;

      browserManager.createNewWebView(QUrl("https://duckduckgo.com"), false);

      QCOMPARE(browserManager.webViewCount(), 2);
      std::vector<QUrl> urls = {QUrl(BrowserManager::NewtabURL),
                                QUrl("https://duckduckgo.com")};
      QCOMPARE(browserManager.webViewUrls(), urls);
      QCOMPARE(browserManager.currentWebViewIndex(), 0);
      QCOMPARE(browserManager.currentUrl(), BrowserManager::NewtabURL);
    }

    context("when creating a new webview with a url and focus is true");
    it("opens the given url as current view") {
      BrowserManager browserManager;

      browserManager.createNewWebView(QUrl("https://duckduckgo.com"), true);

      QCOMPARE(browserManager.webViewCount(), 2);
      std::vector<QUrl> urls = {QUrl(BrowserManager::NewtabURL),
                                QUrl("https://duckduckgo.com")};
      QCOMPARE(browserManager.webViewUrls(), urls);
      QCOMPARE(browserManager.currentWebViewIndex(), 1);
      QCOMPARE(browserManager.currentUrl(), QUrl("https://duckduckgo.com"));
    }
  }

  void testNextNavigation() {
    context("when nextWebView is called");
    context("- and there is only 1 tab");
    it("does nothing") {
      BrowserManager browserManager;

      browserManager.nextWebView();

      QCOMPARE(browserManager.currentWebViewIndex(), 0);
      QCOMPARE(browserManager.currentUrl(), BrowserManager::NewtabURL);
    }

    context("when nextWebView is called");
    context("- and there are tabs after the current tab");
    it("goes to the next tab") {
      BrowserManager browserManager;
      browserManager.createNewWebView(QUrl("http://a1.com"));
      browserManager.createNewWebView(QUrl("http://a2.com"));

      browserManager.nextWebView();

      QCOMPARE(browserManager.currentWebViewIndex(), 1);
      QCOMPARE(browserManager.currentUrl(), QUrl("http://a1.com"));
    }

    context("when nextWebView is called");
    context("- and current tab is the last tab");
    it("jumps to the first tab") {
      BrowserManager browserManager;
      browserManager.createNewWebView(QUrl("http://a1.com"));
      browserManager.createNewWebView(QUrl("http://a2.com"), true);
      QCOMPARE(browserManager.currentWebViewIndex(), 2);

      browserManager.nextWebView();

      QCOMPARE(browserManager.currentWebViewIndex(), 0);
      QCOMPARE(browserManager.currentUrl(), BrowserManager::NewtabURL);
    }
  }

  void testPreviousNavigation() {
    context("when previousWebView is called");
    context("- and there is only 1 tab");
    it("does nothing") {
      BrowserManager browserManager;

      browserManager.previousWebView();

      QCOMPARE(browserManager.currentWebViewIndex(), 0);
      QCOMPARE(browserManager.currentUrl(), BrowserManager::NewtabURL);
    }

    context("when previousWebView is called");
    context("- and there are tabs before the current tab");
    it("goes to the next tab") {
      BrowserManager browserManager;
      browserManager.createNewWebView(QUrl("http://a1.com"));
      browserManager.createNewWebView(QUrl("http://a2.com"), true);
      QCOMPARE(browserManager.currentWebViewIndex(), 2);

      browserManager.previousWebView();

      QCOMPARE(browserManager.currentWebViewIndex(), 1);
      QCOMPARE(browserManager.currentUrl(), QUrl("http://a1.com"));
    }

    context("when previousWebView is called");
    context("- and current tab is the last tab");
    it("jumps to the last tab") {
      BrowserManager browserManager;
      browserManager.createNewWebView(QUrl("http://a1.com"));
      browserManager.createNewWebView(QUrl("http://a2.com"));

      browserManager.previousWebView();

      QCOMPARE(browserManager.currentWebViewIndex(), 2);
      QCOMPARE(browserManager.currentUrl(), QUrl("http://a2.com"));
    }
  }

  void testCloseWebView() {
    context("when closeWebView is called");
    context("- with out of bounds index");
    it("does nothing") {
      BrowserManager browserManager;
      browserManager.setCurrentUrl(QUrl("https://a.com"));

      browserManager.closeWebView(1);

      QCOMPARE(browserManager.webViewCount(), 1);
      QCOMPARE(browserManager.webViewUrls(),
               (std::vector<QUrl>{QUrl("https://a.com")}));
      QCOMPARE(browserManager.currentWebViewIndex(), 0);
      QCOMPARE(browserManager.currentUrl(), QUrl("https://a.com"));
    }

    context("when closeWebView is called");
    context("- and there is only 1 tab");
    it("closes the tab and opens empty tab in its place") {
      BrowserManager browserManager;
      browserManager.setCurrentUrl(QUrl("https://a.com"));
      QCOMPARE(browserManager.webViewCount(), 1);

      browserManager.closeWebView(0);

      QCOMPARE(browserManager.webViewCount(), 1);
      QCOMPARE(browserManager.webViewUrls(),
               (std::vector<QUrl>{BrowserManager::NewtabURL}));
      QCOMPARE(browserManager.currentWebViewIndex(), 0);
      QCOMPARE(browserManager.currentUrl(), BrowserManager::NewtabURL);
    }

    context("when closeWebView is called");
    context("- with the current tab index");
    context("- and there are some tabs after");
    it("closes the tab and focuses the next tab") {
      BrowserManager browserManager;
      browserManager.createNewWebView(QUrl("https://a1.com"), true);
      browserManager.createNewWebView(QUrl("https://a2.com"));
      QCOMPARE(browserManager.webViewCount(), 3);
      QCOMPARE(browserManager.currentWebViewIndex(), 1);

      browserManager.closeWebView(1);

      QCOMPARE(browserManager.webViewCount(), 2);
      QCOMPARE(browserManager.webViewUrls(),
               (std::vector<QUrl>{BrowserManager::NewtabURL,
                                  QUrl("https://a2.com")}));
      QCOMPARE(browserManager.currentWebViewIndex(), 1);
      QCOMPARE(browserManager.currentUrl(), QUrl("https://a2.com"));
    }

    context("when closeWebView is called");
    context("- with the current tab index");
    context("- which is the last tab");
    it("closes the tab and focusses previous tab") {
      BrowserManager browserManager;
      browserManager.createNewWebView(QUrl("https://a1.com"));
      browserManager.createNewWebView(QUrl("https://a2.com"), true);
      QCOMPARE(browserManager.webViewCount(), 3);
      QCOMPARE(browserManager.currentWebViewIndex(), 2);

      browserManager.closeWebView(2);

      QCOMPARE(browserManager.webViewCount(), 2);
      QCOMPARE(browserManager.webViewUrls(),
               (std::vector<QUrl>{BrowserManager::NewtabURL,
                                  QUrl("https://a1.com")}));
      QCOMPARE(browserManager.currentWebViewIndex(), 1);
      QCOMPARE(browserManager.currentUrl(), QUrl("https://a1.com"));
    }
  }

  void testNewWindowRequestSignal() {
    context("when webview emits a newWindowRequested signal");
    context("- of type new tab");
    it("opens a new web view and focusses it") {
      BrowserManager browserManager;
      browserManager.setCurrentUrl(QUrl("https://a.com"));
      auto webview = browserManager.findChild<QWebEngineView *>();

      FakeNewWindowRequest windowRequest(
          FakeNewWindowRequest::DestinationType::InNewTab, QRect(0, 0, 0, 0),
          QUrl("https://new.com"), true);
      emit webview->page()->newWindowRequested(windowRequest);

      QCOMPARE(browserManager.webViewCount(), 2);
      QCOMPARE(
          browserManager.webViewUrls(),
          (std::vector<QUrl>{QUrl("https://a.com"), QUrl("https://new.com")}));
      QCOMPARE(browserManager.currentWebViewIndex(), 1);
      QCOMPARE(browserManager.currentUrl(), QUrl("https://new.com"));
    }

    context("when webview emits a newWindowRequested signal");
    context("- of type new background tab");
    it("opens a new web view in the background") {
      BrowserManager browserManager;
      browserManager.setCurrentUrl(QUrl("https://a.com"));
      auto webview = browserManager.findChild<QWebEngineView *>();

      FakeNewWindowRequest windowRequest(
          FakeNewWindowRequest::DestinationType::InNewBackgroundTab,
          QRect(0, 0, 0, 0), QUrl("https://new.com"), true);
      emit webview->page()->newWindowRequested(windowRequest);

      QCOMPARE(browserManager.webViewCount(), 2);
      QCOMPARE(
          browserManager.webViewUrls(),
          (std::vector<QUrl>{QUrl("https://a.com"), QUrl("https://new.com")}));
      QCOMPARE(browserManager.currentWebViewIndex(), 0);
      QCOMPARE(browserManager.currentUrl(), QUrl("https://a.com"));
    }
  }
};

QTEST_REGISTER(BrowserManagerSpec)
#include "BrowserManagerSpec.moc"
