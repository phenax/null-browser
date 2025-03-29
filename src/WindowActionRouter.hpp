#pragma once

#include <QWidget>
#include <QtCore>
#include <cstdint>

#include "widgets/BrowserWindow.hpp"

class WindowActionRouter : public QWidget {
  Q_OBJECT

public:
  static WindowActionRouter *instance() {
    static auto *router = new WindowActionRouter;
    return router;
  }

  void add_window(BrowserWindow *window);
  const WindowMap &windows();

  WebViewId fetch_current_tab_id(WindowId win_id = 0);
  QList<WebViewData> fetch_webview_data_list(WindowId win_id = 0);

protected:
  WindowActionRouter();

private:
  WindowMap window_map;
  uint64_t last_id = 1;
};
