#include <QWebEngineNewWindowRequest>
#include <QWebEngineProfile>
#include <vector>

#include "Configuration.hpp"
#include "testUtils.h"
#include "widgets/WebView.hpp"
#include "widgets/WebViewStack.hpp"

// NOLINTBEGIN
class WebViewStackSpec : public QObject {
  Q_OBJECT

  class FakeNewWindowRequest : public QWebEngineNewWindowRequest {
  public:
    FakeNewWindowRequest(DestinationType t, const QRect &r, const QUrl &u,
                         bool b)
        : QWebEngineNewWindowRequest(t, r, u, b, nullptr) {}
  };

private slots:
  void test_initial_state() {
    context("when initialized");
    it("opens a single tab") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);

      QCOMPARE(web_view_stack.count(), 1);
      QCOMPARE(web_view_stack.current_url(), configuration.new_tab_url);
      web_view_stack.deleteLater();
    }
  }

  void test_open_url() {
    context("when openUrl is called without an open type");
    it("replaces current tab url with newtab url") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);
      web_view_stack.open_url(QUrl("http://a.com"), OpenType::OpenUrl);

      web_view_stack.open_url(QUrl(configuration.new_tab_url));

      QCOMPARE(web_view_stack.count(), 1);
      std::vector<QUrl> urls = {QUrl(configuration.new_tab_url)};
      QCOMPARE(web_view_stack.urls(), urls);
      QCOMPARE(web_view_stack.current_web_view_index(), 0);
      QCOMPARE(web_view_stack.current_url(), configuration.new_tab_url);
    }

    context("when creating a new webview with a url and focus is false");
    it("opens the given url in background") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);

      web_view_stack.open_url(QUrl("https://duckduckgo.com"),
                              OpenType::OpenUrlInBgTab);

      QCOMPARE(web_view_stack.count(), 2);
      std::vector<QUrl> urls = {QUrl(configuration.new_tab_url),
                                QUrl("https://duckduckgo.com")};
      QCOMPARE(web_view_stack.urls(), urls);
      QCOMPARE(web_view_stack.current_web_view_index(), 0);
      QCOMPARE(web_view_stack.current_url(), configuration.new_tab_url);
    }

    context("when creating a new webview with a url and focus is true");
    it("opens the given url as current view") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);

      web_view_stack.open_url(QUrl("https://duckduckgo.com"),
                              OpenType::OpenUrlInTab);

      QCOMPARE(web_view_stack.count(), 2);
      std::vector<QUrl> urls = {QUrl(configuration.new_tab_url),
                                QUrl("https://duckduckgo.com")};
      QCOMPARE(web_view_stack.urls(), urls);
      QCOMPARE(web_view_stack.current_web_view_index(), 1);
      QCOMPARE(web_view_stack.current_url(), QUrl("https://duckduckgo.com"));
    }
  }

  void test_next_navigation() {
    context("when nextWebView is called");
    context("- and there is only 1 tab");
    it("does nothing") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);

      web_view_stack.next();

      QCOMPARE(web_view_stack.current_web_view_index(), 0);
      QCOMPARE(web_view_stack.current_url(), configuration.new_tab_url);
    }

    context("when nextWebView is called");
    context("- and there are tabs after the current tab");
    it("goes to the next tab") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);
      web_view_stack.open_url(QUrl("http://a1.com"), OpenType::OpenUrlInBgTab);
      web_view_stack.open_url(QUrl("http://a2.com"), OpenType::OpenUrlInBgTab);

      web_view_stack.next();

      QCOMPARE(web_view_stack.current_web_view_index(), 1);
      QCOMPARE(web_view_stack.current_url(), QUrl("http://a1.com"));
    }

    context("when nextWebView is called");
    context("- and current tab is the last tab");
    it("jumps to the first tab") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);
      web_view_stack.open_url(QUrl("http://a1.com"), OpenType::OpenUrlInBgTab);
      web_view_stack.open_url(QUrl("http://a2.com"), OpenType::OpenUrlInTab);
      QCOMPARE(web_view_stack.current_web_view_index(), 2);

      web_view_stack.next();

      QCOMPARE(web_view_stack.current_web_view_index(), 0);
      QCOMPARE(web_view_stack.current_url(), configuration.new_tab_url);
    }
  }

  void test_previous_navigation() {
    context("when previousWebView is called");
    context("- and there is only 1 tab");
    it("does nothing") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);

      web_view_stack.previous();

      QCOMPARE(web_view_stack.current_web_view_index(), 0);
      QCOMPARE(web_view_stack.current_url(), configuration.new_tab_url);
    }

    context("when previousWebView is called");
    context("- and there are tabs before the current tab");
    it("goes to the next tab") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);
      web_view_stack.open_url(QUrl("http://a1.com"), OpenType::OpenUrlInBgTab);
      web_view_stack.open_url(QUrl("http://a2.com"), OpenType::OpenUrlInTab);
      QCOMPARE(web_view_stack.current_web_view_index(), 2);

      web_view_stack.previous();

      QCOMPARE(web_view_stack.current_web_view_index(), 1);
      QCOMPARE(web_view_stack.current_url(), QUrl("http://a1.com"));
    }

    context("when previousWebView is called");
    context("- and current tab is the last tab");
    it("jumps to the last tab") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);
      web_view_stack.open_url(QUrl("http://a1.com"), OpenType::OpenUrlInBgTab);
      web_view_stack.open_url(QUrl("http://a2.com"), OpenType::OpenUrlInBgTab);

      web_view_stack.previous();

      QCOMPARE(web_view_stack.current_web_view_index(), 2);
      QCOMPARE(web_view_stack.current_url(), QUrl("http://a2.com"));
    }
  }

  void test_close_web_view() {
    context("when closeWebView is called");
    context("- with out of bounds index");
    it("does nothing") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);
      web_view_stack.open_url(QUrl("https://a.com"), OpenType::OpenUrl);

      web_view_stack.close(1);

      QCOMPARE(web_view_stack.count(), 1);
      QCOMPARE(web_view_stack.urls(),
               (std::vector<QUrl>{QUrl("https://a.com")}));
      QCOMPARE(web_view_stack.current_web_view_index(), 0);
      QCOMPARE(web_view_stack.current_url(), QUrl("https://a.com"));
    }

    context("when closeWebView is called");
    context("- and there is only 1 tab");
    it("closes the tab and opens empty tab in its place") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);
      web_view_stack.open_url(QUrl("https://a.com"), OpenType::OpenUrl);
      QCOMPARE(web_view_stack.count(), 1);

      web_view_stack.close(0);

      QCOMPARE(web_view_stack.count(), 1);
      QCOMPARE(web_view_stack.urls(),
               (std::vector<QUrl>{configuration.new_tab_url}));
      QCOMPARE(web_view_stack.current_web_view_index(), 0);
      QCOMPARE(web_view_stack.current_url(), configuration.new_tab_url);
    }

    context("when closeWebView is called");
    context("- with the current tab index");
    context("- and there are some tabs after");
    it("closes the tab and focuses the next tab") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);
      web_view_stack.open_url(QUrl("https://a1.com"), OpenType::OpenUrlInTab);
      web_view_stack.open_url(QUrl("https://a2.com"), OpenType::OpenUrlInBgTab);
      QCOMPARE(web_view_stack.count(), 3);
      QCOMPARE(web_view_stack.current_web_view_index(), 1);

      web_view_stack.close(1);

      QCOMPARE(web_view_stack.count(), 2);
      QCOMPARE(web_view_stack.urls(),
               (std::vector<QUrl>{configuration.new_tab_url,
                                  QUrl("https://a2.com")}));
      QCOMPARE(web_view_stack.current_web_view_index(), 1);
      QCOMPARE(web_view_stack.current_url(), QUrl("https://a2.com"));
    }

    context("when closeWebView is called");
    context("- with the current tab index");
    context("- which is the last tab");
    it("closes the tab and focusses previous tab") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);
      web_view_stack.open_url(QUrl("https://a1.com"), OpenType::OpenUrlInBgTab);
      web_view_stack.open_url(QUrl("https://a2.com"), OpenType::OpenUrlInTab);
      QCOMPARE(web_view_stack.count(), 3);
      QCOMPARE(web_view_stack.current_web_view_index(), 2);

      web_view_stack.close(2);

      QCOMPARE(web_view_stack.count(), 2);
      QCOMPARE(web_view_stack.urls(),
               (std::vector<QUrl>{configuration.new_tab_url,
                                  QUrl("https://a1.com")}));
      QCOMPARE(web_view_stack.current_web_view_index(), 1);
      QCOMPARE(web_view_stack.current_url(), QUrl("https://a1.com"));
    }
  }

  void test_new_window_request_signal() {
    context("when webview emits a newWindowRequested signal");
    context("- of type new tab");
    it("opens a new web view and focusses it") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);
      web_view_stack.open_url(QUrl("https://a.com"), OpenType::OpenUrl);
      auto *webview = web_view_stack.findChild<WebView *>();
      QCOMPARE(web_view_stack.count(), 1);

      FakeNewWindowRequest window_request(
          FakeNewWindowRequest::DestinationType::InNewTab, QRect(0, 0, 0, 0),
          QUrl("https://new.com"), true);
      emit webview->page()->newWindowRequested(window_request);

      QCOMPARE(web_view_stack.count(), 2);
      QCOMPARE(
          web_view_stack.urls(),
          (std::vector<QUrl>{QUrl("https://a.com"), QUrl("https://new.com")}));
      QCOMPARE(web_view_stack.current_web_view_index(), 1);
      QCOMPARE(web_view_stack.current_url(), QUrl("https://new.com"));
    }

    context("when webview emits a newWindowRequested signal");
    context("- of type new background tab");
    it("opens a new web view in the background") {
      Configuration configuration;
      WebViewStack web_view_stack(&configuration);
      web_view_stack.open_url(QUrl("https://a.com"), OpenType::OpenUrl);
      auto *webview = web_view_stack.findChild<WebView *>();

      FakeNewWindowRequest window_request(
          FakeNewWindowRequest::DestinationType::InNewBackgroundTab,
          QRect(0, 0, 0, 0), QUrl("https://new.com"), true);
      emit webview->page()->newWindowRequested(window_request);

      QCOMPARE(web_view_stack.count(), 2);
      QCOMPARE(
          web_view_stack.urls(),
          (std::vector<QUrl>{QUrl("https://a.com"), QUrl("https://new.com")}));
      QCOMPARE(web_view_stack.current_web_view_index(), 0);
      QCOMPARE(web_view_stack.current_url(), QUrl("https://a.com"));
    }
  }
};

QTEST_REGISTER(WebViewStackSpec)
#include "WebViewStackSpec.moc"
// NOLINTEND
