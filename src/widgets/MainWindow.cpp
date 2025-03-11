#include <QKeyEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QtCore/qnamespace.h>

#include "CommandParser.hpp"
#include "completion/CommandsAdapter.hpp"
#include "completion/UrlAdapter.hpp"
#include "widgets/InputLine.hpp"
#include "widgets/MainWindow.hpp"
#include "widgets/WebViewStack.hpp"

MainWindow::MainWindow() {
  setStyleSheet("background-color: #000; color: #fff;");
  setCentralWidget(new QWidget());

  // Root stacked layout
  layout = new QStackedLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->setStackingMode(QStackedLayout::StackAll);
  centralWidget()->setLayout(layout);

  // Web engine
  webViewStack = new WebViewStack(new QWebEngineProfile("web-browser"));
  layout->addWidget(webViewStack);

  // Command input
  inputLine = new InputLine;
  inputLine->setHidden(true);
  inputLine->move(0, 0);
  connect(inputLine, &InputLine::submitted, this, &MainWindow::onInputSubmit);
  connect(inputLine, &InputLine::cancelled, this, &MainWindow::hideInputLine);
  layout->addWidget(inputLine);

  // Lua runtime
  luaRuntime = LuaRuntime::instance();
  connect(luaRuntime, &LuaRuntime::urlOpened, this,
          [this](QString url, OpenType openType) {
            webViewStack->openUrl(QUrl(url), openType);
          });
}

void MainWindow::hideInputLine() {
  inputLine->setInputFocus(false);
  inputLine->setHidden(true);
}

void MainWindow::showInputLine() {
  inputLine->setHidden(false);
  layout->setCurrentWidget(inputLine);
  inputLine->setInputFocus(true);
}

void MainWindow::showURLInput(QString url, OpenType openType) {
  setEvaluationType(new UrlEval(openType));
  inputLine->setAdapter(new UrlAdapter);
  inputLine->setInputText(url);
  showInputLine();
}

void MainWindow::showCommandInput(QString cmd) {
  setEvaluationType(new CommandEval());
  inputLine->setAdapter(new CommandsAdapter);
  inputLine->setInputText(cmd);
  showInputLine();
}
void MainWindow::setEvaluationType(EvaluationType *evalType) {
  if (currentEvaluationType)
    delete currentEvaluationType;
  currentEvaluationType = evalType;
}

void MainWindow::onInputSubmit(QString input) {
  hideInputLine();

  if (dynamic_cast<CommandEval *>(currentEvaluationType)) {
    evaluateCommand(input);
  } else if (auto urlEval = dynamic_cast<UrlEval *>(currentEvaluationType)) {
    webViewStack->openUrl(input, urlEval->type());
  }
}

void MainWindow::evaluateCommand(QString command) {
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
      webViewStack->openUrl(cmd.argsString, OpenType::OpenUrl);
    break;
  case CommandType::TabOpen:
    if (cmd.argsString.trimmed().isEmpty())
      showURLInput("", OpenType::OpenUrlInTab);
    else
      webViewStack->openUrl(cmd.argsString, OpenType::OpenUrlInTab);
    break;
  case CommandType::TabNext:
    webViewStack->next();
    break;
  case CommandType::TabPrev:
    webViewStack->previous();
    break;
  case CommandType::Noop:
    break;
  }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  auto combo = event->keyCombination();
  if (combo.key() == Qt::Key_L &&
      combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    showURLInput(webViewStack->currentUrl().toString(), OpenType::OpenUrl);
  } else if (combo.key() == Qt::Key_Semicolon &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    showCommandInput("");
  } else if (combo.key() == Qt::Key_T &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    webViewStack->openUrl(QUrl("https://lite.duckduckgo.com"),
                          OpenType::OpenUrlInTab);
  } else if (combo.key() == Qt::Key_J &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    webViewStack->next();
  } else if (combo.key() == Qt::Key_K &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    webViewStack->previous();
  } else if (combo.key() == Qt::Key_W &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    webViewStack->closeCurrent();
  }
}
