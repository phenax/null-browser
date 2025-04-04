#include <QKeyEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QtCore>

#include "Configuration.hpp"
#include "LuaRuntime.hpp"
#include "WindowMediator.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "widgets/BrowserWindow.hpp"
#include "widgets/WebViewStack.hpp"

BrowserWindow::BrowserWindow(const Configuration &configuration,
                             const QString &start_url)
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

  auto &keymap_evaluator = KeymapEvaluator::instance();

  // TODO: remove
  keymap_evaluator.add_keymap(KeyMode::Normal, "i", [&keymap_evaluator]() {
    keymap_evaluator.set_current_mode(KeyMode::Insert);
  });
  keymap_evaluator.add_keymap(KeyMode::Insert, "<esc>", [&keymap_evaluator]() {
    keymap_evaluator.set_current_mode(KeyMode::Normal);
  });
  keymap_evaluator.add_keymap(KeyMode::Normal, "<c-t>a",
                              []() { qDebug() << "Stuff"; });

  win_mediator = new WindowMediator(web_view_stack);

  connect(web_view_stack, &WebViewStack::current_webview_changed, this,
          [this](int index) {
            auto webviews = win_mediator->get_webview_list();
            if (index >= 0 && index < webviews.count()) {
              const auto &webview = webviews.at(index);
              setWindowTitle(webview.title);
            }
          });

  // TODO: remove
  auto &lua = LuaRuntime::instance();
  lua.queue_task([this, start_url]() {
    auto def_url = start_url.isEmpty()
                       ? "https://github.com/phenax/null-browser"
                       : start_url;
    emit win_mediator->url_opened(def_url, OpenType::OpenUrlInTab, 0);
    emit win_mediator->url_opened("https://ediblemonad.dev",
                                  OpenType::OpenUrlInBgTab, 0);
    emit win_mediator->url_opened("https://github.com/trending",
                                  OpenType::OpenUrlInBgTab, 0);
  });
}

void BrowserWindow::closeEvent(QCloseEvent * /*event*/) { emit closed(); }

bool BrowserWindow::on_window_key_event(QKeyEvent *event) {
  auto &keymap_evaluator = KeymapEvaluator::instance();
  const bool should_skip =
      keymap_evaluator.evaluate(event->modifiers(), (Qt::Key)event->key());

  return should_skip;
}
