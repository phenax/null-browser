#pragma once

#include <QMainWindow>

#include "Configuration.hpp"
#include "WindowMediator.hpp"
#include "utils.hpp"

using WindowId = uint64_t;

class BrowserWindow : public QMainWindow {
public:
  BrowserWindow(const Configuration &configuration);

  DEFINE_GETTER(mediator, input_mediator)
  DEFINE_GETTER(get_id, win_id)
  DEFINE_SETTER(set_id, win_id)

  bool on_window_key_event(QKeyEvent *event);

private:
  WindowMediator *input_mediator;
  const Configuration &configuration;

  WindowId win_id = -1;
};

using WindowMap = std::unordered_map<uint64_t, BrowserWindow *>;
