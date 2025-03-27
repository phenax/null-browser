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
      WebViewStack webview_stack(&configuration);

      QCOMPARE(webview_stack.count(), 1);
      QCOMPARE(webview_stack.current_url(), configuration.new_tab_url);
      webview_stack.deleteLater();
    }
  }

  void test_open_url() {
    context("when openUrl is called without an open type");
    it("replaces current tab url with newtab url") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("http://a.com"), OpenType::OpenUrl);

      webview_stack.open_url(QUrl(configuration.new_tab_url));

      QCOMPARE(webview_stack.count(), 1);
      std::vector<QUrl> urls = {QUrl(configuration.new_tab_url)};
      QCOMPARE(webview_stack.urls(), urls);
      QCOMPARE(webview_stack.current_webview_index(), 0);
      QCOMPARE(webview_stack.current_url(), configuration.new_tab_url);
    }

    context("when creating a new webview with a url and focus is false");
    it("opens the given url in background") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);

      webview_stack.open_url(QUrl("https://duckduckgo.com"),
                             OpenType::OpenUrlInBgTab);

      QCOMPARE(webview_stack.count(), 2);
      std::vector<QUrl> urls = {QUrl(configuration.new_tab_url),
                                QUrl("https://duckduckgo.com")};
      QCOMPARE(webview_stack.urls(), urls);
      QCOMPARE(webview_stack.current_webview_index(), 0);
      QCOMPARE(webview_stack.current_url(), configuration.new_tab_url);
    }

    context("when creating a new webview with a url and focus is true");
    it("opens the given url as current view") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);

      webview_stack.open_url(QUrl("https://duckduckgo.com"),
                             OpenType::OpenUrlInTab);

      QCOMPARE(webview_stack.count(), 2);
      std::vector<QUrl> urls = {QUrl(configuration.new_tab_url),
                                QUrl("https://duckduckgo.com")};
      QCOMPARE(webview_stack.urls(), urls);
      QCOMPARE(webview_stack.current_webview_index(), 1);
      QCOMPARE(webview_stack.current_url(), QUrl("https://duckduckgo.com"));
    }
  }

  // void test_next_navigation() {
  //   context("when nextWebView is called");
  //   context("- and there is only 1 tab");
  //   it("does nothing") {
  //     Configuration configuration;
  //     WebViewStack webview_stack(&configuration);
  //
  //     webview_stack.next();
  //
  //     QCOMPARE(webview_stack.current_webview_index(), 0);
  //     QCOMPARE(webview_stack.current_url(), configuration.new_tab_url);
  //   }
  //
  //   context("when nextWebView is called");
  //   context("- and there are tabs after the current tab");
  //   it("goes to the next tab") {
  //     Configuration configuration;
  //     WebViewStack webview_stack(&configuration);
  //     webview_stack.open_url(QUrl("http://a1.com"),
  //     OpenType::OpenUrlInBgTab);
  //     webview_stack.open_url(QUrl("http://a2.com"),
  //     OpenType::OpenUrlInBgTab);
  //
  //     webview_stack.next();
  //
  //     QCOMPARE(webview_stack.current_webview_index(), 1);
  //     QCOMPARE(webview_stack.current_url(), QUrl("http://a1.com"));
  //   }
  //
  //   context("when nextWebView is called");
  //   context("- and current tab is the last tab");
  //   it("jumps to the first tab") {
  //     Configuration configuration;
  //     WebViewStack webview_stack(&configuration);
  //     webview_stack.open_url(QUrl("http://a1.com"),
  //     OpenType::OpenUrlInBgTab);
  //     webview_stack.open_url(QUrl("http://a2.com"), OpenType::OpenUrlInTab);
  //     QCOMPARE(webview_stack.current_webview_index(), 2);
  //
  //     webview_stack.next();
  //
  //     QCOMPARE(webview_stack.current_webview_index(), 0);
  //     QCOMPARE(webview_stack.current_url(), configuration.new_tab_url);
  //   }
  // }
  //
  // void test_previous_navigation() {
  //   context("when previousWebView is called");
  //   context("- and there is only 1 tab");
  //   it("does nothing") {
  //     Configuration configuration;
  //     WebViewStack webview_stack(&configuration);
  //
  //     webview_stack.previous();
  //
  //     QCOMPARE(webview_stack.current_webview_index(), 0);
  //     QCOMPARE(webview_stack.current_url(), configuration.new_tab_url);
  //   }
  //
  //   context("when previousWebView is called");
  //   context("- and there are tabs before the current tab");
  //   it("goes to the next tab") {
  //     Configuration configuration;
  //     WebViewStack webview_stack(&configuration);
  //     webview_stack.open_url(QUrl("http://a1.com"),
  //     OpenType::OpenUrlInBgTab);
  //     webview_stack.open_url(QUrl("http://a2.com"), OpenType::OpenUrlInTab);
  //     QCOMPARE(webview_stack.current_webview_index(), 2);
  //
  //     webview_stack.previous();
  //
  //     QCOMPARE(webview_stack.current_webview_index(), 1);
  //     QCOMPARE(webview_stack.current_url(), QUrl("http://a1.com"));
  //   }
  //
  //   context("when previousWebView is called");
  //   context("- and current tab is the last tab");
  //   it("jumps to the last tab") {
  //     Configuration configuration;
  //     WebViewStack webview_stack(&configuration);
  //     webview_stack.open_url(QUrl("http://a1.com"),
  //     OpenType::OpenUrlInBgTab);
  //     webview_stack.open_url(QUrl("http://a2.com"),
  //     OpenType::OpenUrlInBgTab);
  //
  //     webview_stack.previous();
  //
  //     QCOMPARE(webview_stack.current_webview_index(), 2);
  //     QCOMPARE(webview_stack.current_url(), QUrl("http://a2.com"));
  //   }
  // }

  void test_close() {
    context("when close is called");
    context("- with invalid id");
    it("does nothing") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("https://a.com"), OpenType::OpenUrl);

      webview_stack.close(99);

      QCOMPARE(webview_stack.count(), 1);
      QCOMPARE(webview_stack.urls(),
               (std::vector<QUrl>{QUrl("https://a.com")}));
      QCOMPARE(webview_stack.current_webview_index(), 0);
      QCOMPARE(webview_stack.current_url(), QUrl("https://a.com"));
    }

    context("when close is called on current webview");
    context("- and there is only 1 tab");
    it("closes the tab and opens empty tab in its place") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("https://a.com"), OpenType::OpenUrl);
      QCOMPARE(webview_stack.count(), 1);

      webview_stack.close(webview_stack.current_webview_id());

      QCOMPARE(webview_stack.count(), 1);
      QCOMPARE(webview_stack.urls(),
               (std::vector<QUrl>{configuration.new_tab_url}));
      QCOMPARE(webview_stack.current_webview_index(), 0);
      QCOMPARE(webview_stack.current_url(), configuration.new_tab_url);
    }

    context("when close is called");
    context("- with the current tab id");
    context("- and there are some tabs after");
    it("closes the tab and focuses the next tab") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("https://a1.com"), OpenType::OpenUrlInTab);
      webview_stack.open_url(QUrl("https://a2.com"), OpenType::OpenUrlInBgTab);
      QCOMPARE(webview_stack.count(), 3);
      QCOMPARE(webview_stack.current_webview_index(), 1);

      webview_stack.close(webview_stack.current_webview_id());

      QCOMPARE(webview_stack.count(), 2);
      QCOMPARE(webview_stack.urls(),
               (std::vector<QUrl>{configuration.new_tab_url,
                                  QUrl("https://a2.com")}));
      QCOMPARE(webview_stack.current_webview_index(), 1);
      QCOMPARE(webview_stack.current_url(), QUrl("https://a2.com"));
    }

    context("when close is called");
    context("- with the current tab id");
    context("- which is the last tab");
    it("closes the tab and focuses previous tab") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("https://a1.com"), OpenType::OpenUrlInBgTab);
      webview_stack.open_url(QUrl("https://a2.com"), OpenType::OpenUrlInTab);
      QCOMPARE(webview_stack.count(), 3);
      QCOMPARE(webview_stack.current_webview_index(), 2);

      webview_stack.close(webview_stack.current_webview_id());

      QCOMPARE(webview_stack.count(), 2);
      QCOMPARE(webview_stack.urls(),
               (std::vector<QUrl>{configuration.new_tab_url,
                                  QUrl("https://a1.com")}));
      QCOMPARE(webview_stack.current_webview_index(), 1);
      QCOMPARE(webview_stack.current_url(), QUrl("https://a1.com"));
    }
  }

  void test_new_window_request_signal() {
    context("when webview emits a newWindowRequested signal");
    context("- of type new tab");
    it("opens a new web view and focusses it") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("https://a.com"), OpenType::OpenUrl);
      auto *webview = webview_stack.findChild<WebView *>();
      QCOMPARE(webview_stack.count(), 1);

      FakeNewWindowRequest window_request(
          FakeNewWindowRequest::DestinationType::InNewTab, QRect(0, 0, 0, 0),
          QUrl("https://new.com"), true);
      emit webview->page()->newWindowRequested(window_request);

      QCOMPARE(webview_stack.count(), 2);
      QCOMPARE(
          webview_stack.urls(),
          (std::vector<QUrl>{QUrl("https://a.com"), QUrl("https://new.com")}));
      QCOMPARE(webview_stack.current_webview_index(), 1);
      QCOMPARE(webview_stack.current_url(), QUrl("https://new.com"));
    }

    context("when webview emits a newWindowRequested signal");
    context("- of type new background tab");
    it("opens a new web view in the background") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("https://a.com"), OpenType::OpenUrl);
      auto *webview = webview_stack.findChild<WebView *>();

      FakeNewWindowRequest window_request(
          FakeNewWindowRequest::DestinationType::InNewBackgroundTab,
          QRect(0, 0, 0, 0), QUrl("https://new.com"), true);
      emit webview->page()->newWindowRequested(window_request);

      QCOMPARE(webview_stack.count(), 2);
      QCOMPARE(
          webview_stack.urls(),
          (std::vector<QUrl>{QUrl("https://a.com"), QUrl("https://new.com")}));
      QCOMPARE(webview_stack.current_webview_index(), 0);
      QCOMPARE(webview_stack.current_url(), QUrl("https://a.com"));
    }
  }
};

QTEST_REGISTER(WebViewStackSpec)
#include "WebViewStackSpec.moc"
// NOLINTEND
