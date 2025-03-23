#pragma once

#include <QMainWindow>

#include "Configuration.hpp"
#include "InputMediator.hpp"

class MainWindow : public QMainWindow {
public:
  MainWindow();

protected:
  bool eventFilter(QObject *object, QEvent *event) override;

private:
  InputMediator *input_mediator;
  Configuration configuration;
};
