#include <QKeyEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QWebEngineView>

#include "BrowserManager.hpp"
#include "CommandInput.hpp"
#include "MainWindow.hpp"

MainWindow::MainWindow() {
  setStyleSheet("background-color: #000; color: #fff;");
  setCentralWidget(new QWidget());

  // Root stacked layout
  auto layout = new QStackedLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->setStackingMode(QStackedLayout::StackAll);
  centralWidget()->setLayout(layout);

  // Web engine
  browserManager = new BrowserManager(new QWebEngineProfile("web-browser"));
  layout->addWidget(browserManager);

  // Command input
  commandInput = new CommandInput(browserManager->currentUrl().toString());
  hideURLInput();
  commandInput->move(0, 0);
  connect(commandInput, &CommandInput::submitted, this,
          &MainWindow::evaluateCommand);
  connect(commandInput, &CommandInput::cancelled, this,
          &MainWindow::hideURLInput);
  layout->addWidget(commandInput);
}

void MainWindow::hideURLInput() {
  commandInput->setInputFocus(false);
  commandInput->setHidden(true);
}

void MainWindow::showURLInput() {
  commandInput->setInputText(browserManager->currentUrl().toString());
  commandInput->raise();
  commandInput->setHidden(false);
  commandInput->setInputFocus(true);
}

void MainWindow::evaluateCommand(QString command) {
  if (!command.isEmpty())
    browserManager->setCurrentUrl(command);
  hideURLInput();
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
