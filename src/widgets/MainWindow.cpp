#include <QKeyEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QtCore/qnamespace.h>

#include "InputMediator.hpp"
#include "widgets/InputLine.hpp"
#include "widgets/MainWindow.hpp"
#include "widgets/WebViewStack.hpp"

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
  auto webViewStack = new WebViewStack(new QWebEngineProfile("web-browser"));
  layout->addWidget(webViewStack);

  // Command input
  auto inputLine = new InputLine;
  layout->addWidget(inputLine);

  inputMediator =
      new InputMediator(inputLine, webViewStack, LuaRuntime::instance());
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  auto combo = event->keyCombination();

  if (combo.key() == Qt::Key_L &&
      combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    inputMediator->showURLInput(inputMediator->currentUrl().toString(),
                                OpenType::OpenUrl);
  } else if (combo.key() == Qt::Key_Semicolon &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    inputMediator->showCommandInput("");
  } else if (combo.key() == Qt::Key_T &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    inputMediator->openUrl(QUrl("https://lite.duckduckgo.com"),
                           OpenType::OpenUrlInTab);
  } else if (combo.key() == Qt::Key_J &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    inputMediator->nextWebView();
  } else if (combo.key() == Qt::Key_K &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    inputMediator->previousWebView();
  } else if (combo.key() == Qt::Key_W &&
             combo.keyboardModifiers().testFlag(Qt::ControlModifier)) {
    inputMediator->closeCurrentWebView();
  }
}
