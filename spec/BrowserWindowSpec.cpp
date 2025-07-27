#include <QWebEngineNewWindowRequest>
#include <QWebEngineProfile>
#include <qwebengineprofile.h>
#include <vector>

#include "Configuration.hpp"
#include "LuaRuntime.hpp"
#include "testUtils.h"
#include "widgets/Decorations.hpp"
#include "widgets/WebView.hpp"
#include "widgets/WebViewStack.hpp"

// NOLINTBEGIN
class BrowserWindowSpec : public QObject {
  Q_OBJECT

  // class FakeNewWindowRequest : public QWebEngineNewWindowRequest {
  // public:
  //   FakeNewWindowRequest(DestinationType t, const QRect &r, const QUrl &u, bool b)
  //       : QWebEngineNewWindowRequest(t, r, u, b, nullptr) {}
  // };

  QWebEngineProfile *profile() { return new QWebEngineProfile(); }

private slots:
  void beforeTestCase() { LuaRuntime::instance().start_event_loop(); }
  void cleanupTestCase() { LuaRuntime::instance().stop_event_loop(); }

  void test_initial_state() {
    describe("constructor");

    context("when initialized with some urls");
    it("opens those urls in stack") {
      Configuration configuration;
      QStringList urls{"https://example1.com", "https://example2.com"};

      BrowserWindow browser_window(configuration, profile(), urls);

      auto *webview_stack = browser_window.findChild<WebViewStack *>();
      QCOMPARE(webview_stack->count(), 2);
      std::vector<QUrl> expected_urls{QUrl("https://example1.com"), QUrl("https://example2.com")};
      QCOMPARE(webview_stack->urls(), expected_urls);
    }

    context("when initialized with no urls");
    it("opens configured new_view_url in stack") {
      Configuration configuration;

      BrowserWindow browser_window(configuration, profile());

      auto *webview_stack = browser_window.findChild<WebViewStack *>();
      QCOMPARE(webview_stack->count(), 1);
      QCOMPARE(webview_stack->urls(), {QUrl(configuration.new_view_url())});
    }
  }

  // void test_window_title_update() {
  //   context("when title is updated");
  //   it("opens configured new_view_url in stack") {
  //     Configuration configuration;
  //     BrowserWindow browser_window(configuration, profile(),
  //                                  {"https://one.com", "https://two.com"});
  //     auto *webview_stack = browser_window.findChild<WebViewStack *>();
  //     QSignalSpy current_webview_title_changed(webview_stack,
  //                                              &WebViewStack::current_webview_title_changed);
  //
  //     emit webview_stack->current_webview_title_changed(0);
  //     QVERIFY(current_webview_title_changed.wait(100));
  //
  //     qDebug() << browser_window.windowTitle();
  //     // QCOMPARE(webview_stack->count(), 1);
  //     // QCOMPARE(webview_stack->urls(), {QUrl(configuration.new_view_url())});
  //   }
  // }
};

QTEST_REGISTER(BrowserWindowSpec)
#include "BrowserWindowSpec.moc"
// NOLINTEND
