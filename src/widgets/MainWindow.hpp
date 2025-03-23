#pragma once

#include <QMainWindow>

#include "Configuration.hpp"
#include "InputMediator.hpp"

class MainWindow : public QMainWindow {
public:
  MainWindow();

private:
  bool eventFilter(QObject *object, QEvent *event) override;

private:
  InputMediator *inputMediator;
  Configuration configuration;
};
