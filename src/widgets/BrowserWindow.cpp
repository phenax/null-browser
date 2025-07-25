#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QtCore>

#include "Configuration.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "widgets/Decorations.hpp"
#include "widgets/WebViewStack.hpp"

#include "widgets/BrowserWindow.hpp"

BrowserWindow::BrowserWindow(const Configuration &configuration, QWebEngineProfile *profile,
                             const QStringList &urls)
    : QMainWindow(nullptr), configuration(configuration) {
  setCentralWidget(new QWidget());

  // Root stacked layout
  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  centralWidget()->setLayout(layout);

  // Stack of web views
  webview_stack = new WebViewStack(&configuration, profile, this);
  decorations = new Decorations(webview_stack, profile, this);
  layout->addWidget(decorations);

  // Open webviews for given urls
  if (urls.isEmpty()) {
    webview_stack->open_url(configuration.new_view_url());
  } else {
    for (const auto &url : urls) {
      webview_stack->open_url(url, OpenType::OpenUrlInView);
    }
  }

  // Default keymaps
  auto &keymap = KeymapEvaluator::instance();
  keymap.define_mode("n", {.passthrough = false});
  keymap.define_mode("i", {.passthrough = true});

  // Update window title when webview changes
  connect(webview_stack, &WebViewStack::current_webview_title_changed, this, [this](int index) {
    auto webviews = webview_stack->get_webview_list();
    if (index >= 0 && index < webviews.count()) {
      const auto &webview = webviews.at(index);
      setWindowTitle(webview.title);
    }
  });

  connect(webview_stack, &WebViewStack::new_window_requested, this,
          &BrowserWindow::new_window_requested);
  connect(webview_stack, &WebViewStack::close_window_requested, this,
          &BrowserWindow::close_window_requested);
}

bool BrowserWindow::on_window_key_event(QKeyEvent *event) {
  auto &keymap_evaluator = KeymapEvaluator::instance();
  const bool should_skip = keymap_evaluator.evaluate(event->modifiers(), (Qt::Key)event->key());

  return should_skip;
}

void BrowserWindow::update_user_agent(const QString &user_agent) {
  auto *profile = webview_stack->get_profile();
  profile->setHttpUserAgent(user_agent);
}

void BrowserWindow::update_downloads_dir(const QString &downloads_dir) {
  auto *profile = webview_stack->get_profile();
  profile->setDownloadPath(downloads_dir);
}

void BrowserWindow::update_permissions_persistance(const QString &persistance) {
  auto *profile = webview_stack->get_profile();
  auto persistance_policy = Configuration::to_permission_persistance_policy(persistance);
  profile->setPersistentPermissionsPolicy(persistance_policy);
}
