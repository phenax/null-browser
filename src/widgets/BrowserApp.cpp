#include <QKeyEvent>
#include <QtCore>

#include "LuaRuntime.hpp"
#include "widgets/BrowserApp.hpp"
#include "widgets/MainWindow.hpp"

BrowserApp::BrowserApp() {
  auto *lua = LuaRuntime::instance();
  lua->start_event_loop();

  // Global event filter
  qApp->installEventFilter(this);

  // NOTE: TMP
  lua->load_file("./config.lua");
};

MainWindow *BrowserApp::create_window() {
  auto *win = new MainWindow((const Configuration &)configuration);
  win->setWindowTitle("null-browser");
  windows.insert({last_id, win});
  last_id++;
  win->show();
  return win;
}

bool BrowserApp::eventFilter(QObject *target, QEvent *event) {
  if (event->type() != QEvent::KeyPress)
    return false;

  for (auto &match : windows) {
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
