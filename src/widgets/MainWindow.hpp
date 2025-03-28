#pragma once

#include <QMainWindow>

#include "Configuration.hpp"
#include "InputMediator.hpp"

class MainWindow : public QMainWindow {
public:
  MainWindow(const Configuration &configuration);

  bool on_window_key_event(QKeyEvent *event);

private:
  InputMediator *input_mediator;
  const Configuration &configuration;
};
