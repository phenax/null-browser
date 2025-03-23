#include <QKeyEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QtCore>

#include "Configuration.hpp"
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
  auto *layout = new QStackedLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->setStackingMode(QStackedLayout::StackAll);
  centralWidget()->setLayout(layout);

  // Web engine
  auto *web_view_stack =
      new WebViewStack((const Configuration *)&configuration,
                       new QWebEngineProfile("null-browser"));
  layout->addWidget(web_view_stack);

  // TODO: remoev
  web_view_stack->open_url(QUrl("https://duckduckgo.com"), OpenType::OpenUrl);
  web_view_stack->open_url(QUrl("https://ediblemonad.dev"),
                           OpenType::OpenUrlInBgTab);
  web_view_stack->open_url(QUrl("https://github.com/trending"),
                           OpenType::OpenUrlInBgTab);

  auto *keymap_evaluator = new KeymapEvaluator;

  input_mediator = new InputMediator(web_view_stack, LuaRuntime::instance(),
                                     keymap_evaluator);

  // NOTE: TMP
  LuaRuntime::instance()->load_file("./config.lua");

  // TODO: remove
  keymap_evaluator->add_keymap(KeyMode::Normal, "i", [keymap_evaluator]() {
    keymap_evaluator->set_current_mode(KeyMode::Insert);
  });
  keymap_evaluator->add_keymap(KeyMode::Insert, "<esc>", [keymap_evaluator]() {
    keymap_evaluator->set_current_mode(KeyMode::Normal);
  });
  keymap_evaluator->add_keymap(KeyMode::Normal, "<c-t>a",
                               []() { qDebug() << "Stuff"; });
}

bool MainWindow::eventFilter(QObject * /*watched*/, QEvent *event) {
  if (event->type() != QEvent::KeyPress)
    return false;

  auto *key_event = static_cast<QKeyEvent *>(event);
  const bool should_skip = input_mediator->evaluate_keymap(
      key_event->modifiers(), (Qt::Key)key_event->key());

  return should_skip;
}
