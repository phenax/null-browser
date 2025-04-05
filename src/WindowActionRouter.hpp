#pragma once

#include <QWidget>
#include <QtCore>
#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

#include "events.hpp"
#include "widgets/BrowserWindow.hpp"
#include "widgets/WebViewStack.hpp"

#define WITH_WEBVIEW_WINDOW(WEBVIEW_ID, IDENT, BLOCK)                          \
  for (auto &win_match : window_map) {                                         \
    auto *IDENT = win_match.second;                                            \
    if (IDENT->mediator()->has_webview(WEBVIEW_ID)) {                          \
      BLOCK;                                                                   \
    }                                                                          \
  }

class WindowActionRouter : public QWidget {
  Q_OBJECT

public:
  static WindowActionRouter &instance() {
    static WindowActionRouter router;
    return router;
  }

  void initialize();

  void add_window(BrowserWindow *window);
  const WindowMap &windows();

  WebViewId fetch_current_tab_id(WindowId win_id = 0);
  QList<WebViewData> fetch_webview_data_list(WindowId win_id = 0);

  void dispatch_event(BrowserEvent *event);
  void register_event(const EventHandlerRequest &event);

protected:
  WindowActionRouter() = default;

  void add_keymap(const QString &mode_string, const QString &keyseq,
                  std::function<void()> action);

signals:
  void new_window_requested(const QUrl &url);

private:
  WindowMap window_map;
  uint64_t last_id = 1;
  Configuration *configuration;

  // TODO: Split event handling to its own class
  std::mutex events_mutex;
  std::unordered_map<QString, std::vector<EventHandlerRequest>> events;
};
