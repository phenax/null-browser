#include <QKeyEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QtCore>

#include "Configuration.hpp"
#include "LuaRuntime.hpp"
#include "WindowActionRouter.hpp"
#include "WindowMediator.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "widgets/BrowserWindow.hpp"
#include "widgets/WebViewStack.hpp"

BrowserWindow::BrowserWindow(const Configuration &configuration)
    : configuration(configuration) {
  setCentralWidget(new QWidget());

  // Root stacked layout
  auto *layout = new QStackedLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->setStackingMode(QStackedLayout::StackAll);
  centralWidget()->setLayout(layout);

  // Web engine
  auto *web_view_stack =
      new WebViewStack(&configuration, new QWebEngineProfile("null-browser"));
  layout->addWidget(web_view_stack);

  auto *keymap_evaluator = KeymapEvaluator::instance();

  // TODO: remove
  keymap_evaluator->add_keymap(KeyMode::Normal, "i", [keymap_evaluator]() {
    keymap_evaluator->set_current_mode(KeyMode::Insert);
  });
  keymap_evaluator->add_keymap(KeyMode::Insert, "<esc>", [keymap_evaluator]() {
    keymap_evaluator->set_current_mode(KeyMode::Normal);
  });
  keymap_evaluator->add_keymap(KeyMode::Normal, "<c-t>a",
                               []() { qDebug() << "Stuff"; });

  input_mediator = new WindowMediator(web_view_stack);

  auto *lua = LuaRuntime::instance();
  lua->queue_task([this]() {
    emit input_mediator->url_opened("https://github.com/phenax/null-browser",
                                    OpenType::OpenUrl, 0);
    emit input_mediator->url_opened("https://ediblemonad.dev",
                                    OpenType::OpenUrlInBgTab, 0);
    emit input_mediator->url_opened("https://github.com/trending",
                                    OpenType::OpenUrlInBgTab, 0);
  });
}

bool BrowserWindow::on_window_key_event(QKeyEvent *event) {
  auto *keymap_evaluator = KeymapEvaluator::instance();
  const bool should_skip =
      keymap_evaluator->evaluate(event->modifiers(), (Qt::Key)event->key());

  return should_skip;
}
