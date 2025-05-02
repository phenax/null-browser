#include <QDir>
#include <QKeyEvent>
#include <QWebEngineNotification>
#include <QtCore>

#include "LuaRuntime.hpp"
#include "WindowActionRouter.hpp"
#include "events/NotificationReceivedEvent.hpp"
#include "widgets/BrowserApp.hpp"
#include "widgets/BrowserWindow.hpp"

BrowserApp::BrowserApp(Configuration &configuration) : configuration(configuration) {
  auto &lua = LuaRuntime::instance();
  lua.start_event_loop();

  // Router init
  auto &window_action_router = WindowActionRouter::instance();
  window_action_router.initialize(&configuration);

  // Global event filter
  qApp->installEventFilter(this);

  qDebug() << "Config dir:" << configuration.get_config_dir().path();

  // Load lua directory into package path
  lua.append_package_path(configuration.get_config_lua_dir());

  // Load init.lua
  auto lua_init_file = configuration.get_config_lua_init_file();
  if (QFile::exists(lua_init_file)) {
    lua.load_file_sync(lua_init_file);
  } else {
    qWarning() << "Unable to find init.lua:" << lua_init_file;
  }

  // Initializes profile
  for (auto *profile : profiles) {
    profile->setDownloadPath(configuration.downloads_dir());
    profile->setHttpUserAgent(configuration.user_agent());
    profile->setNotificationPresenter([](std::unique_ptr<QWebEngineNotification> notification) {
      auto *event = new NotificationReceivedEvent(std::move(notification));
      WindowActionRouter::instance().dispatch_event(event);
    });
    profile->setPersistentPermissionsPolicy(configuration.permission_persistance_policy());
  }

  connect(&window_action_router, &WindowActionRouter::new_window_requested, this,
          [this](const QUrl &url) { create_window({url.toString()}); });
};

BrowserWindow *BrowserApp::create_window(const QStringList &urls) {
  auto *window = new BrowserWindow((const Configuration &)configuration, &default_profile, urls);
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

    if (auto *target_widget = dynamic_cast<QWidget *>(target); win->isAncestorOf(target_widget)) {
      auto *key_event = static_cast<QKeyEvent *>(event);
      const bool should_skip = win->on_window_key_event(key_event);
      return should_skip;
    }
  }

  return false;
}
