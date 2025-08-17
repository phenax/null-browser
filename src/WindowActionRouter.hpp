#pragma once

#include <QWidget>
#include <QtCore>
#include <cstdint>
#include <functional>
#include <mutex>
#include <optional>
#include <vector>

#include "EventQueue.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "utils.hpp"
#include "widgets/BrowserWindow.hpp"
#include "widgets/Decorations.hpp"

#define WITH_WEBVIEW_WINDOW(WEBVIEW_ID, IDENT, BLOCK)                                              \
  for (auto &win_match : window_map) {                                                             \
    auto *IDENT = win_match.second;                                                                \
    if (IDENT->has_webview(WEBVIEW_ID)) {                                                          \
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
  const WindowMap &windows() { return window_map; }

  WebViewId fetch_current_view_id(WindowId win_id = 0);
  QList<WebViewData> fetch_webview_data_list(WindowId win_id = 0);
  QVariant fetch_config_value(const QString &key);
  KeyMode fetch_current_mode() const;
  bool fetch_is_decoration_enabled(DecorationType type, WindowId win_id);
  std::optional<WebViewId> fetch_get_decoration_view_id(DecorationType type, WindowId win_id);

  DEFINE_GETTER(fetch_current_search_text, current_search_text)
  DEFINE_SETTER(set_current_search_text, current_search_text)

  DELEGATE(&event_queue, dispatch_event, dispatch_event)
  DELEGATE(&event_queue, register_event, register_event)

protected:
  WindowActionRouter() = default;

  void add_keymap(const QString &mode_string, const QString &keyseq, std::function<void()> action);
  void find_current_search_text(WebViewId webview_id, bool forward);
  std::vector<BrowserWindow *>
  get_relevant_windows(std::optional<WindowId> win_id = std::nullopt,
                       std::optional<WebViewId> view_id = std::nullopt);

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
