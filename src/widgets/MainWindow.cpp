#include <QKeyEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QtCore/qcoreevent.h>
#include <QtCore/qnamespace.h>
#include <QtWidgets/qapplication.h>

#include "InputMediator.hpp"
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

  // TODO: remoev
  webViewStack->openUrl(QUrl("https://duckduckgo.com"), OpenType::OpenUrl);
  webViewStack->openUrl(QUrl("https://ediblemonad.dev"),
                        OpenType::OpenUrlInBgTab);
  webViewStack->openUrl(QUrl("https://github.com/trending"),
                        OpenType::OpenUrlInBgTab);

  inputMediator = new InputMediator(webViewStack, LuaRuntime::instance());

  // TODO: remove
  keymapEvaluator.addKeymap("default", "<c-t>a", "Stuff");
  keymapEvaluator.addKeymap("default", "<c-t>b", "Other stuff");
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  auto combo = event->keyCombination();

  if (combo.key() == Qt::Key_J &&
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
  if (event->type() != QEvent::KeyPress)
    return false;

  auto keyEvent = static_cast<QKeyEvent *>(event);
  keymapEvaluator.evaluate(keyEvent->modifiers(), (Qt::Key)keyEvent->key());

  return true;
}
