#pragma once

#include <QMainWindow>

#include "Configuration.hpp"
#include "InputMediator.hpp"
#include "keymap/KeymapEvaluator.hpp"

class MainWindow : public QMainWindow {
public:
  MainWindow();

private:
  void keyPressEvent(QKeyEvent *event) override;
  bool eventFilter(QObject *object, QEvent *event) override;

private:
  InputMediator *inputMediator;
  Configuration configuration;
  KeymapEvaluator keymapEvaluator;
};
