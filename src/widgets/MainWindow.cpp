#include <QKeyEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QWebEngineView>

#include "CommandParser.hpp"
#include "widgets/BrowserManager.hpp"
#include "widgets/CommandInput.hpp"
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
  commandInput = new CommandInput;
  hideURLInput();
  commandInput->move(0, 0);
  connect(commandInput, &CommandInput::submitted, this,
          &MainWindow::evaluateCommand);
  connect(commandInput, &CommandInput::cancelled, this,
          &MainWindow::hideURLInput);
  layout->addWidget(commandInput);

  // Lua runtime
  luaRuntime = LuaRuntime::instance();
  connect(luaRuntime, &LuaRuntime::urlOpenned, this,
          [this](QString url, OpenType openType) {
            this->browserManager->openUrl(QUrl(url), openType);
          });
}

void MainWindow::hideURLInput() {
  commandInput->setInputFocus(false);
  commandInput->setHidden(true);
}

void MainWindow::showURLInput() {
  commandInput->setInputText("open " + browserManager->currentUrl().toString());
  commandInput->setHidden(false);
  layout->setCurrentWidget(commandInput);
  commandInput->setInputFocus(true);
}

void MainWindow::evaluateCommand(QString command) {
  hideURLInput();

  // TODO: Temporary hack
  CommandParser parser;
  auto cmd = parser.parse(command);

  switch (cmd.command) {
  case CommandType::LuaEval:
    luaRuntime->evaluate(cmd.args.join(" ").toStdString().c_str());
    break;
  case CommandType::Open:
    browserManager->openUrl(cmd.args.first(), OpenType::OpenUrl);
    break;
  case CommandType::TabOpen:
    browserManager->openUrl(cmd.args.at(1), OpenType::OpenUrlInTab);
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
    toggleURLInput();
  } else if (combo.key() == Qt::Key_T &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    browserManager->createNewWebView(QUrl("https://duckduckgo.com"), true);
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

void MainWindow::toggleURLInput() {
  commandInput->isHidden() ? showURLInput() : hideURLInput();
}
