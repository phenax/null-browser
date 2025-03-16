#pragma once

#include <QMainWindow>
#include <QObject>
#include <QStackedLayout>
#include <QWebEngineView>

#include "Configuration.hpp"
#include "InputMediator.hpp"

class MainWindow : public QMainWindow {
public:
  MainWindow();

private:
  void keyPressEvent(QKeyEvent *event) override;

private:
  InputMediator *inputMediator;
  Configuration configuration;
};
