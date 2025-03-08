#include <QKeyEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <iostream>
#include <lua.hpp>

#include "CommandInput.hpp"
#include "MainWindow.hpp"

MainWindow::MainWindow() {
  setStyleSheet("background-color: #000; color: #fff;");

  auto centralWidget = new QWidget();
  setCentralWidget(centralWidget);

  auto layout = new QStackedLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  centralWidget->setLayout(layout);
  layout->setStackingMode(QStackedLayout::StackAll);

  // Webengine
  web = new QWebEngineView();
  web->setUrl(QUrl("https://ediblemonad.dev"));
  layout->addWidget(web);

  // Command input
  commandInput = new CommandInput(web->url().toString());
  hideInput();
  commandInput->move(0, 0);
  connect(commandInput, &CommandInput::submitted, this,
          &MainWindow::evaluateCommand);
  connect(commandInput, &CommandInput::cancelled, this, &MainWindow::hideInput);

  layout->addWidget(commandInput);
}

void MainWindow::hideInput() {
  commandInput->setInputFocus(false);
  commandInput->setHidden(true);
}

void MainWindow::evaluateCommand(QString command) {
  if (!command.isEmpty())
    web->setUrl(command);
  hideInput();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  auto combo = event->keyCombination();
  if (combo.key() == Qt::Key_L &&
      combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    toggleURLInput();
  }
}

void MainWindow::toggleURLInput() {
  auto shouldShow = commandInput->isHidden();
  if (shouldShow) {
    commandInput->setInputText(web->url().toString());
    commandInput->raise();
  }
  commandInput->setHidden(!shouldShow);
  commandInput->setInputFocus(shouldShow);
}
