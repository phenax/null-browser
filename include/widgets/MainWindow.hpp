#pragma once

#include <QMainWindow>
#include <QObject>
#include <QStackedLayout>
#include <QWebEngineView>

#include "LuaRuntime.hpp"
#include "widgets/BrowserManager.hpp"
#include "widgets/InputLine.hpp"

class MainWindow : public QMainWindow {
public:
  MainWindow();

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void evaluateCommand(QString command);
  void hideInputLine();
  void showInputLine();
  void showURLInput(QString url = "");
  void showCommandInput(QString command = "");

private:
  BrowserManager *browserManager;
  InputLine *inputLine;
  LuaRuntime *luaRuntime;
  QStackedLayout *layout;
};
