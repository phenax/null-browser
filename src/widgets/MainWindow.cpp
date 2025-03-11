#include <QKeyEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QtCore/qnamespace.h>

#include "CommandParser.hpp"
#include "completion/CommandsAdapter.hpp"
#include "completion/UrlAdapter.hpp"
#include "widgets/BrowserManager.hpp"
#include "widgets/InputLine.hpp"
#include "widgets/MainWindow.hpp"

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
  browserManager = new BrowserManager(new QWebEngineProfile("web-browser"));
  layout->addWidget(browserManager);

  // Command input
  inputLine = new InputLine;
  inputLine->setHidden(true);
  inputLine->move(0, 0);
  connect(inputLine, &InputLine::submitted, this, &MainWindow::evaluateCommand);
  connect(inputLine, &InputLine::cancelled, this, &MainWindow::hideInputLine);
  layout->addWidget(inputLine);

  // Lua runtime
  luaRuntime = LuaRuntime::instance();
  connect(luaRuntime, &LuaRuntime::urlOpenned, this,
          [this](QString url, OpenType openType) {
            browserManager->openUrl(QUrl(url), openType);
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

void MainWindow::showURLInput(QString url) {
  showInputLine();
  inputLine->setAdapter(new UrlAdapter);
  inputLine->setInputText(url);
}

void MainWindow::showCommandInput(QString cmd) {
  showInputLine();
  inputLine->setAdapter(new CommandsAdapter);
  inputLine->setInputText(cmd);
}

void MainWindow::evaluateCommand(QString command) {
  hideInputLine();

  CommandParser parser;
  auto cmd = parser.parse(command);

  switch (cmd.command) {
  case CommandType::LuaEval:
    luaRuntime->evaluate(cmd.argsString);
    break;
  case CommandType::Open:
    if (cmd.argsString.trimmed().isEmpty()) {
      showURLInput();
    } else {
      browserManager->openUrl(cmd.argsString, OpenType::OpenUrl);
    }
    break;
  case CommandType::TabOpen:
    browserManager->openUrl(cmd.argsString, OpenType::OpenUrlInTab);
    break;
  case CommandType::TabNext:
    browserManager->nextWebView();
    break;
  case CommandType::TabPrev:
    browserManager->previousWebView();
    break;
  case CommandType::Noop:
    break;
  }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  auto combo = event->keyCombination();
  if (combo.key() == Qt::Key_L &&
      combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    showURLInput(browserManager->currentUrl().toString());
  } else if (combo.key() == Qt::Key_Semicolon &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    showCommandInput();
  } else if (combo.key() == Qt::Key_T &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    browserManager->createNewWebView(QUrl("https://lite.duckduckgo.com"), true);
  } else if (combo.key() == Qt::Key_J &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    browserManager->nextWebView();
  } else if (combo.key() == Qt::Key_K &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    browserManager->previousWebView();
  } else if (combo.key() == Qt::Key_W &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    browserManager->closeCurrentWebView();
  }
}
