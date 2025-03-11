#pragma once

#include <QMainWindow>
#include <QObject>
#include <QStackedLayout>
#include <QWebEngineView>

#include "LuaRuntime.hpp"
#include "widgets/BrowserManager.hpp"
#include "widgets/InputLine.hpp"

class EvaluationType {
public:
  EvaluationType() {}
  virtual ~EvaluationType() = default;
};
class NoopEval : public EvaluationType {};

class MainWindow : public QMainWindow {
public:
  MainWindow();

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void onInputSubmit(QString input);
  void evaluateCommand(QString command);
  void hideInputLine();
  void showInputLine();
  void showURLInput(QString url, OpenType openType);
  void showCommandInput(QString command = "");

  void setEvaluationType(EvaluationType *);

private:
  BrowserManager *browserManager;
  InputLine *inputLine;
  LuaRuntime *luaRuntime;
  QStackedLayout *layout;

  EvaluationType *currentEvaluationType = new NoopEval();
};

class UrlEval : public EvaluationType {
public:
  UrlEval(OpenType type) : EvaluationType(), openType(type) {}
  OpenType type() { return openType; }

private:
  OpenType openType;
};

class CommandEval : public EvaluationType {};
