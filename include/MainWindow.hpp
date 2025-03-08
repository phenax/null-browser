#pragma once

#include <QMainWindow>
#include <QObject>
#include <QWebEngineView>

#include "CommandInput.hpp"

class MainWindow : public QMainWindow {
public:
  MainWindow();

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void toggleURLInput();
  void evaluateCommand(QString command);

private:
  QWebEngineView *web;
  CommandInput *urlInputUI;
};
