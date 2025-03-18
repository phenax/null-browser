#pragma once

#include <QWidget>
#include <QtCore>

#include "LuaRuntime.hpp"
#include "utils.hpp"
#include "widgets/WebViewStack.hpp"

class InputMediator : public QObject {
  Q_OBJECT

public:
  InputMediator(WebViewStack *webViewStack, LuaRuntime *luaRuntime);

  DELEGATE(webViewStack, openUrl, openUrl)
  DELEGATE(webViewStack, currentUrl, currentUrl)
  DELEGATE(webViewStack, next, nextWebView)
  DELEGATE(webViewStack, previous, previousWebView)
  DELEGATE(webViewStack, closeCurrent, closeCurrentWebView)

private:
  void evaluateCommand(QString command);

private:
  WebViewStack *webViewStack;
  LuaRuntime *luaRuntime;
};
