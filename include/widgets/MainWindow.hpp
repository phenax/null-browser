#pragma once

#include <QMainWindow>
#include <QObject>
#include <QStackedLayout>
#include <QWebEngineView>

#include "LuaRuntime.hpp"
#include "widgets/BrowserManager.hpp"
#include "widgets/CommandInput.hpp"

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
  LuaRuntime *luaRuntime;
  QStackedLayout *layout;
};
