#include "InputMediator.hpp"
#include "LuaRuntime.hpp"
#include "testUtils.h"
#include "widgets/InputLine.hpp"
#include "widgets/WebViewStack.hpp"

class InputMediatorSpec : public QObject {
  Q_OBJECT

private slots:
  void testInputTypes() {
    context("when showCommandInput is called");
    it("shows url input with default command") {
      InputLine inputLine;
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      auto luaRuntime = LuaRuntime::instance();
      InputMediator inputMediator(&inputLine, &webViewStack, luaRuntime);
      QCOMPARE(inputLine.isHidden(), true);

      inputMediator.showCommandInput("open");

      QCOMPARE(inputLine.isHidden(), false);
      QCOMPARE(inputLine.prompt(), "[exec]");
      QCOMPARE(inputLine.getInputText(), "open");
    }

    context("when showURLInput is called");
    it("shows url input with default url") {
      InputLine inputLine;
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      auto luaRuntime = LuaRuntime::instance();
      InputMediator inputMediator(&inputLine, &webViewStack, luaRuntime);
      QCOMPARE(inputLine.isHidden(), true);

      inputMediator.showURLInput("http://a.com");

      QCOMPARE(inputLine.isHidden(), false);
      QCOMPARE(inputLine.prompt(), "[url]");
      QCOMPARE(inputLine.getInputText(), "http://a.com");
    }
  }

  void testCommandEvaluationOpen() {
    context("when command open is executed");
    context("- without args");
    it("opens url input") {
      InputLine inputLine;
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      auto luaRuntime = LuaRuntime::instance();
      InputMediator inputMediator(&inputLine, &webViewStack, luaRuntime);

      inputMediator.showCommandInput();
      emit inputLine.submitted("open");

      QCOMPARE(inputLine.prompt(), "[url]");
    }

    context("when command open is executed");
    context("- with url as an arg");
    it("opens url in current tab") {
      InputLine inputLine;
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      auto luaRuntime = LuaRuntime::instance();
      InputMediator inputMediator(&inputLine, &webViewStack, luaRuntime);

      inputMediator.showCommandInput();
      emit inputLine.submitted("open http://a.com");

      std::vector<QUrl> urls = {QUrl("http://a.com")};
      QCOMPARE(webViewStack.urls(), urls);
      QCOMPARE(webViewStack.currentWebViewIndex(), 0);
    }
  }

  void testCommandEvaluationTabOpen() {
    context("when command tabopen is run");
    context("- without args");
    it("opens url in a new tab") {
      InputLine inputLine;
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      auto luaRuntime = LuaRuntime::instance();
      InputMediator inputMediator(&inputLine, &webViewStack, luaRuntime);

      inputMediator.showCommandInput();
      emit inputLine.submitted("tabopen");

      QCOMPARE(inputLine.prompt(), "[url]");
    }

    context("when command tabopen is executed");
    context("- with url as an arg");
    it("opens url in a new tab") {
      InputLine inputLine;
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      auto luaRuntime = LuaRuntime::instance();
      InputMediator inputMediator(&inputLine, &webViewStack, luaRuntime);

      inputMediator.showCommandInput();
      emit inputLine.submitted("tabopen http://a.com");

      std::vector<QUrl> urls = {QUrl(configuration.newTabUrl),
                                QUrl("http://a.com")};
      QCOMPARE(webViewStack.urls(), urls);
      QCOMPARE(webViewStack.currentWebViewIndex(), 1);
    }
  }

  void testCommandEvaluationTabNextPrev() {
    context("when command tabnext is executed");
    it("jumps to next tab") {
      InputLine inputLine;
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      auto luaRuntime = LuaRuntime::instance();
      InputMediator inputMediator(&inputLine, &webViewStack, luaRuntime);
      webViewStack.openUrl(QUrl("https://a1.com"), OpenType::OpenUrl);
      webViewStack.openUrl(QUrl("https://a2.com"), OpenType::OpenUrlInTab);
      webViewStack.openUrl(QUrl("https://a2.com"), OpenType::OpenUrlInBgTab);
      QCOMPARE(webViewStack.currentWebViewIndex(), 1);

      inputMediator.showCommandInput();
      emit inputLine.submitted("tabnext");

      QCOMPARE(webViewStack.currentWebViewIndex(), 2);
    }

    context("when command tabprev is executed");
    it("jumps to previous tab") {
      InputLine inputLine;
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      auto luaRuntime = LuaRuntime::instance();
      InputMediator inputMediator(&inputLine, &webViewStack, luaRuntime);
      webViewStack.openUrl(QUrl("https://a1.com"), OpenType::OpenUrl);
      webViewStack.openUrl(QUrl("https://a2.com"), OpenType::OpenUrlInTab);
      webViewStack.openUrl(QUrl("https://a2.com"), OpenType::OpenUrlInBgTab);
      QCOMPARE(webViewStack.currentWebViewIndex(), 1);

      inputMediator.showCommandInput();
      emit inputLine.submitted("tabprev");

      QCOMPARE(webViewStack.currentWebViewIndex(), 0);
    }
  }

  void testHideInputLine() {
    context("when hideInputLine is called");
    it("hides input") {
      InputLine inputLine;
      Configuration configuration;
      WebViewStack webViewStack(&configuration);
      auto luaRuntime = LuaRuntime::instance();
      InputMediator inputMediator(&inputLine, &webViewStack, luaRuntime);
      inputMediator.showURLInput();
      QCOMPARE(inputLine.isHidden(), false);

      inputMediator.hideInputLine();

      QCOMPARE(inputLine.isHidden(), true);
    }
  }
};

QTEST_REGISTER(InputMediatorSpec)
#include "InputMediatorSpec.moc"
