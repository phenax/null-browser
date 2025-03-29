#pragma once

#include <QMainWindow>

#include "Configuration.hpp"
#include "WindowMediator.hpp"
#include "utils.hpp"

class BrowserWindow : public QMainWindow {
public:
  BrowserWindow(const Configuration &configuration);

  DEFINE_GETTER(mediator, input_mediator)

  bool on_window_key_event(QKeyEvent *event);

private:
  WindowMediator *input_mediator;
  const Configuration &configuration;
};
