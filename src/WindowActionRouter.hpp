#pragma once

#include "widgets/BrowserWindow.hpp"
#include <QWidget>
#include <QtCore>
#include <cstdint>
#include <unordered_map>

using WindowId = uint64_t;
using WindowMap = std::unordered_map<uint64_t, BrowserWindow *>;

class WindowActionRouter : public QWidget {
  Q_OBJECT

public:
  static WindowActionRouter *instance() {
    static auto *router = new WindowActionRouter;
    return router;
  }

  void add_window(BrowserWindow *window);
  const WindowMap &windows();

protected:
  WindowActionRouter();

private:
  WindowMap window_map;
  uint64_t last_id = 1;
};
