#include <QWidget>
#include <QtCore>
#include <string>
#include <unordered_map>

#include "LuaRuntime.hpp"
#include "WindowActionRouter.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "widgets/BrowserWindow.hpp"
#include "widgets/WebViewStack.hpp"

void WindowActionRouter::dispatch_event(BrowserEvent &event) {
  auto &runtime = LuaRuntime::instance();
  runtime.queue_task([this, &event]() {
    std::unordered_map<std::string, std::vector<EventHandlerRequest>> event_map;
    {
      const std::lock_guard<std::mutex> lock(events_mutex);
      event_map = events;
    }

    if (!event_map.contains(event.kind))
      return;

    for (auto &event_handler : event_map.at(event.kind)) {
      // TODO: Pattern filter
      event_handler.handler(event);
    }
  });
}

void WindowActionRouter::register_event(const EventHandlerRequest &event) {
  const std::lock_guard<std::mutex> lock(events_mutex);
  for (const auto &event_name : event.event_names) {
    if (!events.contains(event_name))
      events.insert({event_name, {}});
    events.at(event_name).emplace_back(event);
  }
}

void WindowActionRouter::initialize() {
  UrlChangedEvent url_changed_event("https://googa.com", 2, 1);
  WindowActionRouter::instance().dispatch_event(url_changed_event);

  UrlChangedEvent url_changed_event_1("https://duckduckgo.com", 5, 1);
  WindowActionRouter::instance().dispatch_event(url_changed_event_1);

  auto &runtime = LuaRuntime::instance();

  connect(&runtime, &LuaRuntime::keymap_added, this,
          &WindowActionRouter::add_keymap);

  connect(&runtime, &LuaRuntime::history_back_requested, this,
          [this](WebViewId webview_id, qsizetype history_index) {
            WITH_WEBVIEW_WINDOW(webview_id, window, {
              emit window->mediator()->history_back_requested(webview_id,
                                                              history_index);
            });
          });
  connect(&runtime, &LuaRuntime::history_forward_requested, this,
          [this](WebViewId webview_id, qsizetype history_index) {
            WITH_WEBVIEW_WINDOW(webview_id, window, {
              emit window->mediator()->history_forward_requested(webview_id,
                                                                 history_index);
            });
          });
  connect(&runtime, &LuaRuntime::url_opened, this,
          [this](const QString &url, OpenType open_type, WebViewId webview_id) {
            WITH_WEBVIEW_WINDOW(webview_id, window, {
              emit window->mediator()->url_opened(url, open_type, webview_id);
            });
          });
  connect(&runtime, &LuaRuntime::webview_closed, this,
          [this](WebViewId webview_id) {
            WITH_WEBVIEW_WINDOW(webview_id, window, {
              emit window->mediator()->webview_closed(webview_id);
            });
          });
  connect(&runtime, &LuaRuntime::webview_selected, this,
          [this](WebViewId webview_id) {
            WITH_WEBVIEW_WINDOW(webview_id, window, {
              emit window->mediator()->webview_selected(webview_id);
            });
          });
}

void WindowActionRouter::add_window(BrowserWindow *window) {
  auto win_id = last_id;
  last_id++;

  window_map.insert({win_id, window});
  window->set_id(win_id);
  connect(window, &BrowserWindow::closed, this,
          [this, win_id]() { window_map.erase(win_id); });
}

const WindowMap &WindowActionRouter::windows() { return window_map; }

void WindowActionRouter::add_keymap(const QString &mode_string,
                                    const QString &keyseq,
                                    std::function<void()> action) {
  auto &keymap_evaluator = KeymapEvaluator::instance();
  const KeyMode mode = keymap_evaluator.mode_from_string(mode_string);
  keymap_evaluator.add_keymap(mode, keyseq, std::move(action));
}

WebViewId WindowActionRouter::fetch_current_tab_id(WindowId win_id) {
  for (auto &pair : window_map) {
    auto *win = pair.second;
    auto is_current_window =
        win_id == win->get_id() || (win_id == 0 && win->isActiveWindow());
    if (is_current_window) {
      return win->mediator()->current_webview_id();
    }
  }
  return 0;
}

QList<WebViewData>
WindowActionRouter::fetch_webview_data_list(WindowId win_id) {
  for (auto &pair : window_map) {
    auto *win = pair.second;
    auto is_current_window =
        win_id == win->get_id() || (win_id == 0 && win->isActiveWindow());
    if (is_current_window) {
      return win->mediator()->get_webview_list();
    }
  }
  return {};
}
