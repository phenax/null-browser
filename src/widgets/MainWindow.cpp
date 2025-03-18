#include <QKeyEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QtCore/qcoreevent.h>
#include <QtCore/qnamespace.h>
#include <QtWidgets/qapplication.h>

#include "InputMediator.hpp"
#include "widgets/InputLine.hpp"
#include "widgets/MainWindow.hpp"
#include "widgets/WebViewStack.hpp"

MainWindow::MainWindow() {
  setStyleSheet("background-color: #000; color: #fff;");
  setCentralWidget(new QWidget()); // TODO: manage widget memory

  qApp->installEventFilter(this);

  // Root stacked layout
  auto layout = new QStackedLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->setStackingMode(QStackedLayout::StackAll);
  centralWidget()->setLayout(layout);

  // Web engine
  auto webViewStack = new WebViewStack((const Configuration *)&configuration,
                                       new QWebEngineProfile("null-browser"));
  layout->addWidget(webViewStack);

  // Command input
  auto inputLine = new InputLine();
  layout->addWidget(inputLine);

  inputMediator =
      new InputMediator(inputLine, webViewStack, LuaRuntime::instance());

  keymapEvaluator.addKeymap("default", "<c-t>a", "Stuff");
  keymapEvaluator.addKeymap("default", "<c-t>b", "Other stuff");
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
    inputMediator->showTabsInput();
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

bool MainWindow::eventFilter(QObject *object, QEvent *event) {
  if (event->type() != QEvent::KeyPress && event->type() != QEvent::KeyRelease)
    return false;

  if (auto keyEvent = dynamic_cast<QKeyEvent *>(event)) {
    if (keyEvent->type() == QEvent::KeyPress)
      keymapEvaluator.evaluate(keyEvent->modifiers(), (Qt::Key)keyEvent->key());

    // qDebug() << "EV SELF: " << (object == this) << " | " << event->type();
    // qDebug() << "Key: " << keyEvent->modifiers() << keyEvent->key();
    return true;
  }

  return false;
}
