#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QtCore>
#include <qcoreevent.h>

#include "Configuration.hpp"
#include "WindowActionRouter.hpp"
#include "events/KeyPressedEvent.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "widgets/BrowserApp.hpp"
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

  // Stack of web views + decorations
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

bool BrowserWindow::on_window_key_event(QObject *target, QKeyEvent *event) {
  auto &keymap_evaluator = KeymapEvaluator::instance();
  const bool should_skip = keymap_evaluator.evaluate(event->modifiers(), (Qt::Key)event->key());

  // TODO: Fix this logic to find the right "target" for event
  if (event->type() == QEvent::KeyPress && dynamic_cast<WebView *>(target->parent())) {
    auto *lua_event = KeyPressedEvent::from_qkeyevent(event);
    WindowActionRouter::instance().dispatch_event(lua_event);
  }

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

IWebViewMediator *BrowserWindow::get_webview_mediator(WebViewId webview_id) {
  if (decorations->has_webview(webview_id))
    return decorations;
  return webview_stack;
}

bool BrowserWindow::has_webview(WebViewId webview_id) {
  return webview_stack->has_webview(webview_id) || decorations->has_webview(webview_id);
}

void BrowserWindow::open_url(const QUrl &url, OpenType open_type, WebViewId webview_id) {
  get_webview_mediator(webview_id)->open_url(url, open_type, webview_id);
}

void BrowserWindow::set_html(const QString &html, WebViewId webview_id) {
  get_webview_mediator(webview_id)->set_html(html, webview_id);
}

void BrowserWindow::run_javascript(const QString &js_code, WebViewId webview_id) {
  get_webview_mediator(webview_id)->run_javascript(js_code, webview_id);
}

void BrowserWindow::expose_rpc_function(const QString &name, const RpcFunc &action,
                                        WebViewId webview_id) {
  get_webview_mediator(webview_id)->expose_rpc_function(name, action, webview_id);
}
