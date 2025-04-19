#pragma once

#include <QWidget>
#include <QtCore>
#include <cstdint>
#include <functional>
#include <mutex>

#include "EventQueue.hpp"
#include "widgets/BrowserWindow.hpp"
#include "widgets/WebViewStack.hpp"

#define WITH_WEBVIEW_WINDOW(WEBVIEW_ID, IDENT, BLOCK)                                              \
  for (auto &win_match : window_map) {                                                             \
    auto *IDENT = win_match.second;                                                                \
    if (IDENT->mediator()->has_webview(WEBVIEW_ID)) {                                              \
      BLOCK;                                                                                       \
    }                                                                                              \
  }

class WindowActionRouter : public QWidget {
  Q_OBJECT

public:
  static WindowActionRouter &instance() {
    static WindowActionRouter router;
    return router;
  }

  void initialize(Configuration *config);

  void add_window(BrowserWindow *window);
  const WindowMap &windows();

  WebViewId fetch_current_view_id(WindowId win_id = 0);
  QList<WebViewData> fetch_webview_data_list(WindowId win_id = 0);
  QVariant fetch_config_value(const QString &key);
  QString fetch_current_search_text() const;

  DELEGATE((&event_queue), dispatch_event, dispatch_event);
  DELEGATE((&event_queue), register_event, register_event)

protected:
  WindowActionRouter() = default;

  void add_keymap(const QString &mode_string, const QString &keyseq, std::function<void()> action);

signals:
  void new_window_requested(const QUrl &url);

private:
  std::mutex window_map_mutex;
  WindowMap window_map;
  uint64_t last_id = 1;
  Configuration *configuration;

  EventQueue event_queue;
  QString current_search_text;
};
