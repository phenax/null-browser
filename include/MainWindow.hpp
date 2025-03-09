#pragma once

#include <QMainWindow>
#include <QObject>
#include <QWebEngineView>

#include "BrowserManager.hpp"
#include "CommandInput.hpp"

class MainWindow : public QMainWindow {
public:
  MainWindow();

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void toggleURLInput();
  void evaluateCommand(QString command);
  void hideURLInput();
  void showURLInput();

private:
  BrowserManager *browserManager;
  CommandInput *commandInput;
};
