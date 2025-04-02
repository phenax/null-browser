#include <QKeyEvent>
#include <QtCore>

#include "LuaRuntime.hpp"
#include "WindowActionRouter.hpp"
#include "widgets/BrowserApp.hpp"
#include "widgets/BrowserWindow.hpp"

BrowserApp::BrowserApp() {
  auto &lua = LuaRuntime::instance();
  lua.start_event_loop();

  // Router init
  WindowActionRouter::instance().initialize();

  // Global event filter
  qApp->installEventFilter(this);

  // NOTE: TMP
  lua.load_file("./config.lua");
};

BrowserWindow *BrowserApp::create_window() {
  auto *window = new BrowserWindow((const Configuration &)configuration);
  window->setWindowTitle("null-browser");
  WindowActionRouter::instance().add_window(window);
  window->show();
  return window;
}

bool BrowserApp::eventFilter(QObject *target, QEvent *event) {
  // TODO: Prevent key release and shortcut on mode too
  if (event->type() != QEvent::KeyPress)
    return false;

  for (const auto &match : WindowActionRouter::instance().windows()) {
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
