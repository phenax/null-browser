#include <QWidget>
#include <QtCore>

#include "LuaRuntime.hpp"
#include "WindowActionRouter.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "widgets/WebViewStack.hpp"

void WindowActionRouter::initialize() {
  auto *runtime = LuaRuntime::instance();

  connect(runtime, &LuaRuntime::keymap_added, this,
          &WindowActionRouter::add_keymap);

  connect(runtime, &LuaRuntime::history_back_requested, this,
          [this](WebViewId webview_id, qsizetype history_index) {
            for (auto &win : window_map) {
              auto *mediator = win.second->mediator();
              if (mediator->has_webview(webview_id)) {
                emit mediator->history_back_requested(webview_id,
                                                      history_index);
              }
            }
          });
  connect(runtime, &LuaRuntime::history_forward_requested, this,
          [this](WebViewId webview_id, qsizetype history_index) {
            for (auto &win : window_map) {
              auto *mediator = win.second->mediator();
              if (mediator->has_webview(webview_id)) {
                emit mediator->history_forward_requested(webview_id,
                                                         history_index);
              }
            }
          });

  connect(runtime, &LuaRuntime::url_opened, this,
          [this](const QString &url, OpenType open_type, WebViewId webview_id) {
            for (auto &win : window_map) {
              auto *mediator = win.second->mediator();
              if (mediator->has_webview(webview_id)) {
                emit mediator->url_opened(url, open_type, webview_id);
              }
            }
          });
  connect(runtime, &LuaRuntime::webview_closed, this,
          [this](WebViewId webview_id) {
            for (auto &win : window_map) {
              auto *mediator = win.second->mediator();
              if (mediator->has_webview(webview_id)) {
                emit mediator->webview_closed(webview_id);
              }
            }
          });
  connect(runtime, &LuaRuntime::webview_selected, this,
          [this](WebViewId webview_id) {
            for (auto &win : window_map) {
              auto *mediator = win.second->mediator();
              if (mediator->has_webview(webview_id)) {
                emit mediator->webview_selected(webview_id);
              }
            }
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
  qDebug() << "ADD KEY" << mode_string << keyseq;
  auto *keymap_evaluator = KeymapEvaluator::instance();
  const KeyMode mode = keymap_evaluator->mode_from_string(mode_string);
  keymap_evaluator->add_keymap(mode, keyseq, std::move(action));
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
