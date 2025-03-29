#include <QWidget>
#include <QtCore>

#include "LuaRuntime.hpp"
#include "WindowActionRouter.hpp"
#include "widgets/WebViewStack.hpp"

WindowActionRouter::WindowActionRouter() {
  auto *runtime = LuaRuntime::instance();

  connect(runtime, &LuaRuntime::keymap_added, this,
          [this](const QString &mode, const QString &keyseq,
                 const std::function<void()> &callback) {
            for (auto &win : window_map) {
              auto *mediator = win.second->mediator();
              emit mediator->keymap_added(mode, keyseq, callback);
            }
          });

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
  window_map.insert({last_id, window});
  last_id++;
}

const WindowMap &WindowActionRouter::windows() { return window_map; }
