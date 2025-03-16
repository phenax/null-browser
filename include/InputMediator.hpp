#pragma once

#include <QWidget>
#include <QtCore>

#include "LuaRuntime.hpp"
#include "utils.hpp"
#include "widgets/InputLine.hpp"
#include "widgets/WebViewStack.hpp"

class EvaluationType {
public:
  EvaluationType() {}
  virtual ~EvaluationType() = default;
};
class NoopEval : public EvaluationType {};

class InputMediator : public QObject {
  Q_OBJECT

public:
  InputMediator(InputLine *inputLine, WebViewStack *webViewStack,
                LuaRuntime *luaRuntime);

  void showURLInput(QString url = "", OpenType openType = OpenType::OpenUrl);
  void showCommandInput(QString command = "");
  void showTabsInput(QString command = "");
  void hideInputLine();

  DELEGATE(webViewStack, openUrl, openUrl)
  DELEGATE(webViewStack, currentUrl, currentUrl)
  DELEGATE(webViewStack, next, nextWebView)
  DELEGATE(webViewStack, previous, previousWebView)
  DELEGATE(webViewStack, closeCurrent, closeCurrentWebView)
  DELEGATE(inputLine, getInputText, getInputText)

private:
  void showInputLine();
  void evaluateCommand(QString command);
  void setEvaluationType(EvaluationType *);

private slots:
  void onInputSubmit(QString input);

private:
  InputLine *inputLine;
  WebViewStack *webViewStack;
  LuaRuntime *luaRuntime;
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

class TabsEval : public EvaluationType {};
