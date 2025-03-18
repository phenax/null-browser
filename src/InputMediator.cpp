#include <QList>
#include <QWidget>
#include <QtCore>

#include "CommandParser.hpp"
#include "InputMediator.hpp"
#include "LuaRuntime.hpp"
#include "widgets/WebViewStack.hpp"

InputMediator::InputMediator(WebViewStack *webViewStack, LuaRuntime *luaRuntime,
                             KeymapEvaluator *keymapEvaluator)
    : QObject(), webViewStack(webViewStack), luaRuntime(luaRuntime),
      keymapEvaluator(keymapEvaluator) {
  connect(luaRuntime, &LuaRuntime::urlOpened, webViewStack,
          &WebViewStack::openUrl);
}

void InputMediator::evaluateCommand(QString command) {
  CommandParser parser;
  auto cmd = parser.parse(command);

  switch (cmd.command) {
  case CommandType::LuaEval:
    luaRuntime->evaluate(cmd.argsString);
    break;
  case CommandType::Open:
    openUrl(cmd.argsString, OpenType::OpenUrl);
    break;
  case CommandType::TabOpen:
    openUrl(cmd.argsString, OpenType::OpenUrlInTab);
    break;
  case CommandType::TabNext:
    nextWebView();
    break;
  case CommandType::TabPrev:
    previousWebView();
    break;
  case CommandType::TabSelect:
    webViewStack->focusWebView(cmd.argsString.toLong());
    break;
  case CommandType::Noop:
    break;
  }
}

InputMediator::~InputMediator() { delete webViewStack; }
