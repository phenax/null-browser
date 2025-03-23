#include <QKeyEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QtCore/qcoreevent.h>
#include <QtCore/qnamespace.h>
#include <QtWidgets/qapplication.h>

#include "InputMediator.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "widgets/MainWindow.hpp"
#include "widgets/WebViewStack.hpp"

MainWindow::MainWindow() {
  setStyleSheet("background-color: #000; color: #fff;");
  setCentralWidget(new QWidget()); // TODO: manage widget memory

  // Global event filter
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

  auto keymapEvaluator = new KeymapEvaluator;

  inputMediator =
      new InputMediator(webViewStack, LuaRuntime::instance(), keymapEvaluator);

  // NOTE: TMP
  LuaRuntime::instance()->loadFile("./config.lua");

  // TODO: remove
  keymapEvaluator->addKeymap(KeyMode::Normal, "i", [keymapEvaluator]() {
    keymapEvaluator->setCurrentMode(KeyMode::Insert);
  });
  keymapEvaluator->addKeymap(KeyMode::Insert, "<esc>", [keymapEvaluator]() {
    keymapEvaluator->setCurrentMode(KeyMode::Normal);
  });
  keymapEvaluator->addKeymap(KeyMode::Normal, "<c-t>a",
                             []() { qDebug() << "Stuff"; });
}

bool MainWindow::eventFilter(QObject *, QEvent *event) {
  if (event->type() != QEvent::KeyPress)
    return false;

  auto keyEvent = static_cast<QKeyEvent *>(event);
  bool shouldSkip = inputMediator->evaluateKeymap(keyEvent->modifiers(),
                                                  (Qt::Key)keyEvent->key());

  return shouldSkip;
}
