#include <QKeyEvent>
#include <QtCore>

#include "LuaRuntime.hpp"
#include "WindowActionRouter.hpp"
#include "widgets/BrowserApp.hpp"
#include "widgets/BrowserWindow.hpp"

BrowserApp::BrowserApp() {
  auto *lua = LuaRuntime::instance();
  lua->start_event_loop();

  // Global event filter
  qApp->installEventFilter(this);
};

BrowserWindow *BrowserApp::create_window() {
  auto *win = new BrowserWindow((const Configuration &)configuration);
  win->setWindowTitle("null-browser");
  auto *router = WindowActionRouter::instance();
  router->add_window(win);
  win->show();
  return win;
}

bool BrowserApp::eventFilter(QObject *target, QEvent *event) {
  if (event->type() != QEvent::KeyPress)
    return false;

  for (const auto &match : WindowActionRouter::instance()->windows()) {
    auto *win = match.second;

    if (auto *target_widget = dynamic_cast<QWidget *>(target);
        win->isAncestorOf(target_widget)) {
      auto *key_event = static_cast<QKeyEvent *>(event);
      const bool should_skip = win->on_window_key_event(key_event);
      return should_skip;
    }
  }

  return false;
}
