#include <QKeyEvent>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <iostream>
#include <lua.hpp>

#include "CommandInput.hpp"
#include "MainWindow.hpp"

MainWindow::MainWindow() {
  setStyleSheet("background-color: #000; color: #fff;");
  // layout = new QVBoxLayout(this);
  // layout->setContentsMargins(0, 0, 0, 0);

  web = new QWebEngineView(this);
  web->setGeometry(200, 10, 500, 500);
  web->setUrl(QUrl("https://ediblemonad.dev"));

  urlInputUI = new CommandInput(web->url().toString(), this);
  urlInputUI->setHidden(true);
  connect(urlInputUI, &CommandInput::submitted, this,
          &MainWindow::evaluateCommand);
}

void MainWindow::evaluateCommand(QString command) {
  web->setUrl(command);
  urlInputUI->setInputFocus(false);
  urlInputUI->setHidden(true);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  auto combo = event->keyCombination();
  if (combo.key() == Qt::Key_L &&
      combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    toggleURLInput();
  }
}

void MainWindow::toggleURLInput() {
  auto hidden = urlInputUI->isHidden();
  auto shouldShow = hidden;
  urlInputUI->setHidden(!shouldShow);
  urlInputUI->setInputFocus(shouldShow);
}
