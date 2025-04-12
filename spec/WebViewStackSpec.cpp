#include <QWebEngineNewWindowRequest>
#include <QWebEngineProfile>
#include <vector>

#include "Configuration.hpp"
#include "LuaRuntime.hpp"
#include "testUtils.h"
#include "widgets/WebView.hpp"
#include "widgets/WebViewStack.hpp"

// NOLINTBEGIN
class WebViewStackSpec : public QObject {
  Q_OBJECT

  class FakeNewWindowRequest : public QWebEngineNewWindowRequest {
  public:
    FakeNewWindowRequest(DestinationType t, const QRect &r, const QUrl &u, bool b)
        : QWebEngineNewWindowRequest(t, r, u, b, nullptr) {}
  };

private slots:
  void beforeTestCase() { LuaRuntime::instance().start_event_loop(); }
  void cleanupTestCase() { LuaRuntime::instance().stop_event_loop(); }

  void test_initial_state() {
    context("when initialized");
    it("opens an empty stack maintaining current url") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);

      QCOMPARE(webview_stack.count(), 0);
      QCOMPARE(webview_stack.current_url(), configuration.new_view_url());
    }
  }

  void test_open_url() {
    context("when openUrl is called with an empty stack");
    it("opens a new webview") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      QCOMPARE(webview_stack.count(), 0);

      webview_stack.open_url(QUrl("http://a.com"));

      QCOMPARE(webview_stack.count(), 1);
      QCOMPARE(webview_stack.urls(), (std::vector<QUrl>{QUrl("http://a.com")}));
      QCOMPARE(webview_stack.current_webview_index(), 0);
      QCOMPARE(webview_stack.current_url(), QUrl("http://a.com"));
    }

    context("when openUrl is called with webviews open");
    it("replaces current view url with newview url") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("http://a.com"));
      webview_stack.open_url(QUrl("http://b.com"), OpenType::OpenUrlInView);
      QCOMPARE(webview_stack.count(), 2);
      QCOMPARE(webview_stack.current_webview_index(), 1);

      webview_stack.open_url(QUrl("http://new-b.com"));

      QCOMPARE(webview_stack.count(), 2);
      QCOMPARE(webview_stack.urls(), (std::vector<QUrl>{
                                         QUrl("http://a.com"),
                                         QUrl("http://new-b.com"),
                                     }));
      QCOMPARE(webview_stack.current_webview_index(), 1);
      QCOMPARE(webview_stack.current_url(), QUrl("http://new-b.com"));
    }

    context("when openurl is called with OpenUrlInBgView");
    it("opens the given url in background") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("http://a.com"));

      webview_stack.open_url(QUrl("https://duckduckgo.com"), OpenType::OpenUrlInBgView);

      QCOMPARE(webview_stack.count(), 2);
      QCOMPARE(webview_stack.urls(),
               (std::vector<QUrl>{QUrl("http://a.com"), QUrl("https://duckduckgo.com")}));
      QCOMPARE(webview_stack.current_webview_index(), 0);
      QCOMPARE(webview_stack.current_url(), QUrl("http://a.com"));
    }

    context("when openurl is called with OpenUrlInView");
    it("opens the given url as current view") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("http://a.com"));

      webview_stack.open_url(QUrl("https://duckduckgo.com"), OpenType::OpenUrlInView);

      QCOMPARE(webview_stack.count(), 2);
      QCOMPARE(webview_stack.urls(),
               (std::vector<QUrl>{QUrl("http://a.com"), QUrl("https://duckduckgo.com")}));
      QCOMPARE(webview_stack.current_webview_index(), 1);
      QCOMPARE(webview_stack.current_url(), QUrl("https://duckduckgo.com"));
    }

    context("when openurl is called with OpenUrlInWindow");
    it("requests a new window") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      QSignalSpy new_window_requested_spy(&webview_stack, &WebViewStack::new_window_requested);
      webview_stack.open_url(QUrl("http://a.com"));

      webview_stack.open_url(QUrl("https://duckduckgo.com"), OpenType::OpenUrlInWindow);
      new_window_requested_spy.wait(100);

      QCOMPARE(new_window_requested_spy.count(), 1);
      QCOMPARE(new_window_requested_spy.takeFirst().at(0), QUrl("https://duckduckgo.com"));
      QCOMPARE(webview_stack.count(), 1);
    }
  }

  void test_close() {
    context("when close is called");
    context("- with invalid id");
    it("does nothing") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("https://a.com"));

      webview_stack.close(99);

      QCOMPARE(webview_stack.count(), 1);
      QCOMPARE(webview_stack.urls(), (std::vector<QUrl>{QUrl("https://a.com")}));
      QCOMPARE(webview_stack.current_webview_index(), 0);
      QCOMPARE(webview_stack.current_url(), QUrl("https://a.com"));
    }

    context("when close is called on current webview");
    context("- and there is only 1 view");
    it("requests closing window") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      QSignalSpy close_window_requested_spy(&webview_stack, &WebViewStack::close_window_requested);
      webview_stack.open_url(QUrl("https://a.com"));
      QCOMPARE(webview_stack.count(), 1);

      webview_stack.close(webview_stack.current_webview_id());
      close_window_requested_spy.wait(100);

      QCOMPARE(close_window_requested_spy.count(), 1);
    }

    context("when close is called");
    context("- with the current view id");
    context("- and there are some views after");
    it("closes the view and focuses the next view") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("https://a0.com"));
      webview_stack.open_url(QUrl("https://a1.com"), OpenType::OpenUrlInView);
      webview_stack.open_url(QUrl("https://a2.com"), OpenType::OpenUrlInBgView);
      QCOMPARE(webview_stack.count(), 3);
      QCOMPARE(webview_stack.current_webview_index(), 1);

      webview_stack.close(webview_stack.current_webview_id());

      QCOMPARE(webview_stack.count(), 2);
      QCOMPARE(webview_stack.urls(),
               (std::vector<QUrl>{QUrl("https://a0.com"), QUrl("https://a2.com")}));
      QCOMPARE(webview_stack.current_webview_index(), 1);
      QCOMPARE(webview_stack.current_url(), QUrl("https://a2.com"));
    }

    context("when close is called");
    context("- with the current view id");
    context("- which is the last view");
    it("closes the view and focuses previous view") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("https://a0.com"));
      webview_stack.open_url(QUrl("https://a1.com"), OpenType::OpenUrlInBgView);
      webview_stack.open_url(QUrl("https://a2.com"), OpenType::OpenUrlInView);
      QCOMPARE(webview_stack.count(), 3);
      QCOMPARE(webview_stack.current_webview_index(), 2);

      webview_stack.close(webview_stack.current_webview_id());

      QCOMPARE(webview_stack.count(), 2);
      QCOMPARE(webview_stack.urls(),
               (std::vector<QUrl>{QUrl("https://a0.com"), QUrl("https://a1.com")}));
      QCOMPARE(webview_stack.current_webview_index(), 1);
      QCOMPARE(webview_stack.current_url(), QUrl("https://a1.com"));
    }
  }

  void test_new_window_request_signal() {
    context("when webview emits a newWindowRequested signal");
    context("- of type new view");
    it("opens a new web view and focuses it") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("https://a.com"));
      auto *webview = webview_stack.findChild<WebView *>();
      QCOMPARE(webview_stack.count(), 1);

      FakeNewWindowRequest window_request(FakeNewWindowRequest::DestinationType::InNewTab,
                                          QRect(0, 0, 0, 0), QUrl("https://new.com"), true);
      emit webview->page()->newWindowRequested(window_request);

      QCOMPARE(webview_stack.count(), 2);
      QCOMPARE(webview_stack.urls(),
               (std::vector<QUrl>{QUrl("https://a.com"), QUrl("https://new.com")}));
      QCOMPARE(webview_stack.current_webview_index(), 1);
      QCOMPARE(webview_stack.current_url(), QUrl("https://new.com"));
    }

    context("when webview emits a newWindowRequested signal");
    context("- of type new background view");
    it("opens a new web view in the background") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      webview_stack.open_url(QUrl("https://a.com"));
      auto *webview = webview_stack.findChild<WebView *>();

      FakeNewWindowRequest window_request(FakeNewWindowRequest::DestinationType::InNewBackgroundTab,
                                          QRect(0, 0, 0, 0), QUrl("https://new.com"), true);
      emit webview->page()->newWindowRequested(window_request);

      QCOMPARE(webview_stack.count(), 2);
      QCOMPARE(webview_stack.urls(),
               (std::vector<QUrl>{QUrl("https://a.com"), QUrl("https://new.com")}));
      QCOMPARE(webview_stack.current_webview_index(), 0);
      QCOMPARE(webview_stack.current_url(), QUrl("https://a.com"));
    }

    context("when webview emits a newWindowRequested signal");
    context("- of type new window");
    it("requests a new window") {
      Configuration configuration;
      WebViewStack webview_stack(&configuration);
      QSignalSpy new_window_requested_spy(&webview_stack, &WebViewStack::new_window_requested);
      webview_stack.open_url(QUrl("https://a.com"));
      QCOMPARE(webview_stack.count(), 1);
      auto *webview = webview_stack.findChild<WebView *>();

      FakeNewWindowRequest window_request(FakeNewWindowRequest::DestinationType::InNewWindow,
                                          QRect(0, 0, 0, 0), QUrl("https://new.com"), true);
      emit webview->page()->newWindowRequested(window_request);
      new_window_requested_spy.wait(100);

      QCOMPARE(new_window_requested_spy.count(), 1);
      QCOMPARE(new_window_requested_spy.takeFirst().at(0), QUrl("https://new.com"));
      QCOMPARE(webview_stack.count(), 1);
    }
  }
};

QTEST_REGISTER(WebViewStackSpec)
#include "WebViewStackSpec.moc"
// NOLINTEND
