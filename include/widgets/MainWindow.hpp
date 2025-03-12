#pragma once

#include <QMainWindow>
#include <QObject>
#include <QStackedLayout>
#include <QWebEngineView>

#include "InputMediator.hpp"

class MainWindow : public QMainWindow {
public:
  MainWindow();

private:
  void keyPressEvent(QKeyEvent *event) override;

private:
  InputMediator *inputMediator;
};
