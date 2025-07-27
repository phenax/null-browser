#include <QWebEngineNewWindowRequest>
#include <QWebEngineProfile>
#include <qdeadlinetimer.h>
#include <qsignalspy.h>
#include <qtestcase.h>
#include <qtestsupport_core.h>
#include <qurlquery.h>

#include "LuaRuntime.hpp"
#include "schemes/NullRpcSchemeHandler.hpp"
#include "testUtils.h"
#include "widgets/WebView.hpp"

// NOLINTBEGIN
class WebViewSpec : public QObject {
  Q_OBJECT

  QWebEngineProfile *profile() { return new QWebEngineProfile(); }

private slots:
  void beforeTestCase() { LuaRuntime::instance().start_event_loop(); }
  void cleanupTestCase() { LuaRuntime::instance().stop_event_loop(); }

  void test_rpc_enabled() {
    describe("webview rpc enabled");

    context("when a defined function is called");
    it("calls the function") {
      WebView webview(1, profile());
      webview.enable_rpc_api();
      bool my_func_was_called = false;
      webview.expose_rpc_function(
          "my_func", [&my_func_was_called](RpcArgs /* unused */) { my_func_was_called = true; });

      auto &nullrpc = NullRPCSchemeHandler::instance();
      NullRPCMessage message{.name = "my_func", .params = QUrlQuery()};
      emit nullrpc.message_received(message);

      QVERIFY(QTest::qWaitFor([&my_func_was_called]() { return my_func_was_called; }));
    }

    context("when a defined function is called with arguements");
    it("calls the function with the arguments") {
      WebView webview(1, profile());
      webview.enable_rpc_api();
      bool my_func_was_called = false;
      QString my_func_arg = "";
      webview.expose_rpc_function("my_func", [&my_func_was_called, &my_func_arg](RpcArgs args) {
        my_func_was_called = true;
        my_func_arg = args.contains("my_arg") ? args.at("my_arg").toString() : "";
      });

      auto &nullrpc = NullRPCSchemeHandler::instance();
      NullRPCMessage message{.name = "my_func", .params = QUrlQuery{{"my_arg", "my arg value"}}};
      emit nullrpc.message_received(message);

      QVERIFY(QTest::qWaitFor([&my_func_was_called]() { return my_func_was_called; }));
      QCOMPARE(my_func_arg, "my arg value");
    }

    context("when an undefined function is called");
    it("does nothing") {
      WebView webview(1, profile());
      webview.enable_rpc_api();
      bool my_func_was_called = false;
      webview.expose_rpc_function(
          "my_func", [&my_func_was_called](RpcArgs /* unused */) { my_func_was_called = true; });

      auto &nullrpc = NullRPCSchemeHandler::instance();
      NullRPCMessage message{.name = "my_undefined_func", .params = QUrlQuery()};
      emit nullrpc.message_received(message);

      QVERIFY(NOT QTest::qWaitFor([&my_func_was_called]() { return my_func_was_called; },
                                  QDeadlineTimer(std::chrono::milliseconds(200))));
    }
  }

  void test_rpc_disabled() {
    describe("webview rpc disabled");

    context("when a defined function is called");
    it("does nothing") {
      WebView webview(1, profile());
      bool my_func_was_called = false;
      webview.expose_rpc_function(
          "my_func", [&my_func_was_called](RpcArgs /* unused */) { my_func_was_called = true; });

      auto &nullrpc = NullRPCSchemeHandler::instance();
      NullRPCMessage message{.name = "my_func", .params = QUrlQuery()};
      emit nullrpc.message_received(message);

      QVERIFY(NOT QTest::qWaitFor([&my_func_was_called]() { return my_func_was_called; },
                                  QDeadlineTimer(std::chrono::milliseconds(200))));
    }
  }
};

QTEST_REGISTER(WebViewSpec)
#include "WebViewSpec.moc"
// NOLINTEND
