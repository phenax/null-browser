#include <QWidget>
#include <QtCore>
#include <unordered_map>

#include "LuaRuntime.hpp"
#include "WindowActionRouter.hpp"
#include "WindowMediator.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "widgets/BrowserWindow.hpp"
#include "widgets/WebViewStack.hpp"

QVariant WindowActionRouter::fetch_config_value(const QString &key) {
  if (key == "new_view_url")
    return configuration->new_view_url;
  if (key == "close_window_when_no_views")
    return configuration->close_window_when_no_views;
  return "";
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void WindowActionRouter::initialize(Configuration *config) {
  auto &runtime = LuaRuntime::instance();
  configuration = config;

  connect(&runtime, &LuaRuntime::keymap_added, this, &WindowActionRouter::add_keymap);

  connect(&runtime, &LuaRuntime::config_updated, this,
          [this](const QString &key, const QVariant &value) {
            qDebug() << key << value;
            if (key == "new_view_url") {
              configuration->new_view_url = value.toString();
            } else if (key == "close_window_when_no_views") {
              configuration->close_window_when_no_views = value.toBool();
            }
          });

  connect(&runtime, &LuaRuntime::history_back_requested, this,
          [this](WebViewId webview_id, qsizetype history_index) {
            WITH_WEBVIEW_WINDOW(webview_id, window, {
              emit window->mediator()->history_back_requested(webview_id, history_index);
            });
          });
  connect(&runtime, &LuaRuntime::history_forward_requested, this,
          [this](WebViewId webview_id, qsizetype history_index) {
            WITH_WEBVIEW_WINDOW(webview_id, window, {
              emit window->mediator()->history_forward_requested(webview_id, history_index);
            });
          });
  connect(&runtime, &LuaRuntime::url_opened, this,
          [this](const QString &url, OpenType open_type, WebViewId webview_id) {
            WITH_WEBVIEW_WINDOW(webview_id, window, {
              emit window->mediator()->url_opened(url, open_type, webview_id);
            });
          });
  connect(&runtime, &LuaRuntime::webview_closed, this, [this](WebViewId webview_id) {
    WITH_WEBVIEW_WINDOW(webview_id, window,
                        { emit window->mediator()->webview_closed(webview_id); });
  });
  connect(&runtime, &LuaRuntime::webview_selected, this, [this](WebViewId webview_id) {
    WITH_WEBVIEW_WINDOW(webview_id, window,
                        { emit window->mediator()->webview_selected(webview_id); });
  });
}

void WindowActionRouter::add_window(BrowserWindow *window) {
  auto win_id = last_id;
  last_id++;

  window->set_id(win_id);
  {
    const std::lock_guard<std::mutex> lock(window_map_mutex);
    window_map.insert({win_id, window});
  }

  connect(window, &BrowserWindow::closed, this, [this, window]() {
    window->disconnect();
    LuaRuntime::instance().queue_task([this, window]() {
      const std::lock_guard<std::mutex> lock(window_map_mutex);
      window_map.erase(window->get_id());
    });
  });
  connect(window->mediator(), &WindowMediator::close_window_requested, window,
          [window]() { window->close(); });
  connect(window->mediator(), &WindowMediator::new_window_requested, this,
          &WindowActionRouter::new_window_requested);
}

const WindowMap &WindowActionRouter::windows() { return window_map; }

void WindowActionRouter::add_keymap(const QString &mode_string, const QString &keyseq,
                                    std::function<void()> action) {
  auto &keymap_evaluator = KeymapEvaluator::instance();
  const KeyMode mode = keymap_evaluator.mode_from_string(mode_string);
  keymap_evaluator.add_keymap(mode, keyseq, std::move(action));
}

WebViewId WindowActionRouter::fetch_current_view_id(WindowId win_id) {
  const std::lock_guard<std::mutex> lock(window_map_mutex);
  for (auto &pair : window_map) {
    auto *win = pair.second;
    auto is_current_window = win_id == win->get_id() || (win_id == 0 && win->isActiveWindow());
    if (is_current_window) {
      return win->mediator()->current_webview_id();
    }
  }
  return 0;
}

QList<WebViewData> WindowActionRouter::fetch_webview_data_list(WindowId win_id) {
  const std::lock_guard<std::mutex> lock(window_map_mutex);
  for (auto &pair : window_map) {
    auto *win = pair.second;
    auto is_current_window = win_id == win->get_id() || (win_id == 0 && win->isActiveWindow());
    if (is_current_window) {
      return win->mediator()->get_webview_list();
    }
  }
  return {};
}
