#pragma once

#include <unordered_map>

#include "widgets/MainWindow.hpp"

class BrowserApp : public QObject {
  Q_OBJECT

public:
  BrowserApp();

  MainWindow *create_window();

protected:
  bool eventFilter(QObject *target, QEvent *event) override;

private:
  std::unordered_map<uint64_t, MainWindow *> windows;
  uint64_t last_id = 1;

  Configuration configuration;
};
