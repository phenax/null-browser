#include <QWidget>
#include <QtCore>
#include <unordered_map>

#include "LuaRuntime.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "widgets/BrowserWindow.hpp"
#include "widgets/WebViewStack.hpp"

#include "WindowActionRouter.hpp"

QVariant WindowActionRouter::fetch_config_value(const QString &key) {
  return configuration->get_config(key);
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void WindowActionRouter::initialize(Configuration *config) {
  auto &runtime = LuaRuntime::instance();
  configuration = config;

  connect(&runtime, &LuaRuntime::keymap_add_requested, this, &WindowActionRouter::add_keymap);
  connect(&runtime, &LuaRuntime::keymap_mode_update_requested, this,
          [](const QString &mode) { KeymapEvaluator::instance().set_current_mode(mode); });

  // Configuration
  connect(&runtime, &LuaRuntime::config_updated, configuration, &Configuration::set_config);
  connect(configuration, &Configuration::user_agent_updated, this,
          [this](const QString &user_agent) {
            for (auto &win_match : window_map)
              win_match.second->update_user_agent(user_agent);
          });
  connect(configuration, &Configuration::downloads_dir_updated, this,
          [this](const QString &downloads_dir) {
            for (auto &win_match : window_map)
              win_match.second->update_downloads_dir(downloads_dir);
          });
  connect(configuration, &Configuration::permissions_persistance_updated, this,
          [this](const QString &persistance) {
            for (auto &win_match : window_map)
              win_match.second->update_permissions_persistance(persistance);
          });

  // History
  connect(&runtime, &LuaRuntime::history_back_requested, this,
          [this](WebViewId webview_id, qsizetype history_index) {
            WITH_WEBVIEW_WINDOW(webview_id, window,
                                { window->history_back(webview_id, history_index); });
          });
  connect(&runtime, &LuaRuntime::history_forward_requested, this,
          [this](WebViewId webview_id, qsizetype history_index) {
            WITH_WEBVIEW_WINDOW(webview_id, window,
                                { window->history_forward(webview_id, history_index); });
          });

  // Webview action
  connect(&runtime, &LuaRuntime::url_opened, this,
          [this](const QString &url, OpenType open_type, WebViewId webview_id) {
            WITH_WEBVIEW_WINDOW(webview_id, window,
                                { window->open_url(url, open_type, webview_id); });
          });
  connect(&runtime, &LuaRuntime::webview_closed, this, [this](WebViewId webview_id) {
    WITH_WEBVIEW_WINDOW(webview_id, window, { window->close_webview(webview_id); });
  });
  connect(&runtime, &LuaRuntime::webview_selected, this, [this](WebViewId webview_id) {
    WITH_WEBVIEW_WINDOW(webview_id, window, { window->select_webview(webview_id); });
  });

  // Search
  connect(&runtime, &LuaRuntime::search_requested, this,
          [this](const QString &text, WebViewId webview_id) {
            set_current_search_text(text.trimmed());
            find_current_search_text(webview_id, true);
          });
  connect(&runtime, &LuaRuntime::search_next_requested, this,
          [this](WebViewId webview_id) { find_current_search_text(webview_id, true); });
  connect(&runtime, &LuaRuntime::search_previous_requested, this,
          [this](WebViewId webview_id) { find_current_search_text(webview_id, false); });

  // Devtools
  connect(&runtime, &LuaRuntime::devtools_requested, this, [this](WebViewId webview_id) {
    WITH_WEBVIEW_WINDOW(webview_id, window, { win_match.second->open_devtools(webview_id); })
  });

  // Scroll
  connect(&runtime, &LuaRuntime::webview_scroll_requested, this,
          [this](WebViewId webview_id, int deltax, int deltay) {
            WITH_WEBVIEW_WINDOW(webview_id, window,
                                { window->scroll(webview_id, deltax, deltay); });
          });
  connect(&runtime, &LuaRuntime::webview_scroll_top_requested, this, [this](WebViewId webview_id) {
    WITH_WEBVIEW_WINDOW(webview_id, window, { window->scroll_to_top(webview_id); });
  });
  connect(&runtime, &LuaRuntime::webview_scroll_bottom_requested, this,
          [this](WebViewId webview_id) {
            WITH_WEBVIEW_WINDOW(webview_id, window, { window->scroll_to_bottom(webview_id); });
          });
  connect(&runtime, &LuaRuntime::decoration_set_enabled, this,
          [this](DecorationType type, bool enabled) {
            qDebug() << "routing" << type << enabled << window_map.size();
            for (auto &win_match : window_map)
              win_match.second->set_decoration_enabled(type, enabled);
          });
}

void WindowActionRouter::find_current_search_text(WebViewId webview_id, bool forward) {
  WITH_WEBVIEW_WINDOW(webview_id, window, {
    win_match.second->set_search_text(current_search_text, webview_id, forward);
  })
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
  connect(window, &BrowserWindow::close_window_requested, window, [window]() { window->close(); });
  connect(window, &BrowserWindow::new_window_requested, this,
          &WindowActionRouter::new_window_requested);
}

const WindowMap &WindowActionRouter::windows() { return window_map; }

void WindowActionRouter::add_keymap(const QString &mode_string, const QString &keyseq,
                                    std::function<void()> action) {
  auto &keymap_evaluator = KeymapEvaluator::instance();
  keymap_evaluator.add_keymap(mode_string, keyseq, std::move(action));
}

WebViewId WindowActionRouter::fetch_current_view_id(WindowId win_id) {
  const std::lock_guard<std::mutex> lock(window_map_mutex);
  for (auto &pair : window_map) {
    auto *win = pair.second;
    auto is_current_window = win_id == win->get_id() || (win_id == 0 && win->isActiveWindow());
    if (is_current_window) {
      return win->current_webview_id();
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
      return win->get_webview_list();
    }
  }
  return {};
}

KeyMode WindowActionRouter::fetch_current_mode() const {
  return KeymapEvaluator::instance().get_current_mode();
}
