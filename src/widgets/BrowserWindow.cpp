#include <QKeyEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QtCore>
#include <qmainwindow.h>

#include "Configuration.hpp"
#include "WindowMediator.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "widgets/BrowserWindow.hpp"
#include "widgets/WebViewStack.hpp"

BrowserWindow::BrowserWindow(const Configuration &configuration, const QStringList &urls)
    : QMainWindow(nullptr), configuration(configuration) {
  setCentralWidget(new QWidget());

  // Root stacked layout
  auto *layout = new QStackedLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->setStackingMode(QStackedLayout::StackAll);
  centralWidget()->setLayout(layout);

  // Web engine
  auto *webview_stack = new WebViewStack(&configuration, new QWebEngineProfile("null-browser"));
  layout->addWidget(webview_stack);

  // Open webviews for given urls
  if (urls.isEmpty()) {
    webview_stack->open_url(configuration.new_view_url.toString());
  } else {
    for (const auto &url : urls) {
      webview_stack->open_url(url, OpenType::OpenUrlInView);
    }
  }

  // Default keymaps
  auto &keymap_evaluator = KeymapEvaluator::instance();

  // TODO: remove
  keymap_evaluator.add_keymap(KeyMode::Normal, "i", [&keymap_evaluator]() {
    keymap_evaluator.set_current_mode(KeyMode::Insert);
  });
  keymap_evaluator.add_keymap(KeyMode::Insert, "<esc>", [&keymap_evaluator]() {
    keymap_evaluator.set_current_mode(KeyMode::Normal);
  });
  keymap_evaluator.add_keymap(KeyMode::Normal, "<c-t>a", []() { qDebug() << "Stuff"; });

  win_mediator = new WindowMediator(webview_stack);

  // Update window title when webview changes
  connect(webview_stack, &WebViewStack::current_webview_title_changed, this, [this](int index) {
    auto webviews = win_mediator->get_webview_list();
    if (index >= 0 && index < webviews.count()) {
      const auto &webview = webviews.at(index);
      setWindowTitle(webview.title);
    }
  });
}

void BrowserWindow::closeEvent(QCloseEvent * /*event*/) { emit closed(); }

bool BrowserWindow::on_window_key_event(QKeyEvent *event) {
  auto &keymap_evaluator = KeymapEvaluator::instance();
  const bool should_skip = keymap_evaluator.evaluate(event->modifiers(), (Qt::Key)event->key());

  return should_skip;
}
