#include <QList>
#include <QWidget>
#include <QtCore>

#include "CommandParser.hpp"
#include "InputMediator.hpp"
#include "LuaRuntime.hpp"
#include "completion/CommandsAdapter.hpp"
#include "completion/TabsAdapter.hpp"
#include "completion/UrlAdapter.hpp"
#include "widgets/WebViewStack.hpp"

InputMediator::InputMediator(InputLine *inputLine, WebViewStack *webViewStack,
                             LuaRuntime *luaRuntime)
    : QObject(), inputLine(inputLine), webViewStack(webViewStack),
      luaRuntime(luaRuntime) {
  // Inputline
  inputLine->setHidden(true);
  connect(inputLine, &InputLine::submitted, this,
          &InputMediator::onInputSubmit);
  connect(inputLine, &InputLine::cancelled, this,
          &InputMediator::hideInputLine);

  // Lua runtime
  connect(luaRuntime, &LuaRuntime::urlOpened, webViewStack,
          &WebViewStack::openUrl);
}

void InputMediator::onInputSubmit(QString input) {
  hideInputLine();

  // Evaluate input from command
  if (dynamic_cast<CommandEval *>(currentEvaluationType))
    return evaluateCommand(input);

  // Evaluate input from url
  if (auto urlEval = dynamic_cast<UrlEval *>(currentEvaluationType))
    return webViewStack->openUrl(input, urlEval->type());

  // Evaluate input from tab list
  if (dynamic_cast<TabsEval *>(currentEvaluationType))
    return webViewStack->focusWebView(input.toLong());
}

void InputMediator::hideInputLine() {
  inputLine->setInputFocus(false);
  inputLine->setHidden(true);
}

void InputMediator::showInputLine() {
  inputLine->setHidden(false);
  inputLine->raise();
  inputLine->setInputFocus(true);
}

void InputMediator::showCommandInput(QString cmd) {
  setEvaluationType(new CommandEval());
  inputLine->setAdapter(new CommandsAdapter);
  inputLine->setInputText(cmd);
  showInputLine();
}

void InputMediator::showTabsInput(QString url) {
  setEvaluationType(new TabsEval());
  QList<Tab> tabs = webViewStack->getTabList();
  inputLine->setAdapter(new TabsAdapter(tabs));
  inputLine->setInputText(url);
  showInputLine();
}

void InputMediator::setEvaluationType(EvaluationType *evalType) {
  if (currentEvaluationType)
    delete currentEvaluationType;
  currentEvaluationType = evalType;
}

void InputMediator::showURLInput(QString url, OpenType openType) {
  setEvaluationType(new UrlEval(openType));
  inputLine->setAdapter(new UrlAdapter);
  inputLine->setInputText(url);
  showInputLine();
}

void InputMediator::evaluateCommand(QString command) {
  CommandParser parser;
  auto cmd = parser.parse(command);

  switch (cmd.command) {
  case CommandType::LuaEval:
    luaRuntime->evaluate(cmd.argsString);
    break;
  case CommandType::Open:
    if (cmd.argsString.trimmed().isEmpty())
      showURLInput("", OpenType::OpenUrl);
    else
      openUrl(cmd.argsString, OpenType::OpenUrl);
    break;
  case CommandType::TabOpen:
    if (cmd.argsString.trimmed().isEmpty())
      showURLInput("", OpenType::OpenUrlInTab);
    else
      openUrl(cmd.argsString, OpenType::OpenUrlInTab);
    break;
  case CommandType::TabNext:
    nextWebView();
    break;
  case CommandType::TabPrev:
    previousWebView();
    break;
  case CommandType::TabSelect:
    showTabsInput();
    // focusWebView(long index) // TODO: parse index and select
    break;
  case CommandType::Noop:
    break;
  }
}
