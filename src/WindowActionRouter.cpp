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
  return configuration->get_config(key);
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void WindowActionRouter::initialize(Configuration *config) {
  auto &runtime = LuaRuntime::instance();
  configuration = config;

  connect(&runtime, &LuaRuntime::keymap_added, this, &WindowActionRouter::add_keymap);

  connect(&runtime, &LuaRuntime::config_updated, configuration, &Configuration::set_config);
  connect(configuration, &Configuration::user_agent_updated, this,
          [this](const QString &user_agent) {
            for (auto &win_match : window_map)
              win_match.second->mediator()->update_user_agent(user_agent);
          });
  connect(configuration, &Configuration::downloads_dir_updated, this,
          [this](const QString &downloads_dir) {
            for (auto &win_match : window_map)
              win_match.second->mediator()->update_downloads_dir(downloads_dir);
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
  connect(&runtime, &LuaRuntime::search_requested, this,
          [this](const QString &text, WebViewId webview_id) {
            WITH_WEBVIEW_WINDOW(webview_id, window, {
              auto *mediator = win_match.second->mediator();
              this->current_search_text = text.trimmed();
              mediator->set_search_text(this->current_search_text, webview_id, true);
            })
          });
  connect(&runtime, &LuaRuntime::search_next_requested, this, [this](WebViewId webview_id) {
    WITH_WEBVIEW_WINDOW(webview_id, window, {
      win_match.second->mediator()->set_search_text(this->current_search_text, webview_id, true);
    })
  });
  connect(&runtime, &LuaRuntime::search_previous_requested, this, [this](WebViewId webview_id) {
    WITH_WEBVIEW_WINDOW(webview_id, window, {
      win_match.second->mediator()->set_search_text(this->current_search_text, webview_id, false);
    })
  });
  connect(&runtime, &LuaRuntime::devtools_requested, this, [this](WebViewId webview_id) {
    WITH_WEBVIEW_WINDOW(webview_id, window,
                        { win_match.second->mediator()->open_devtools(webview_id); })
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

QString WindowActionRouter::fetch_current_search_text() const { return current_search_text; }
