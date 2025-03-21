#pragma once

#include <QWidget>
#include <QtCore>

#include "LuaRuntime.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "utils.hpp"
#include "widgets/WebViewStack.hpp"

class InputMediator : public QObject {
  Q_OBJECT

public:
  InputMediator(WebViewStack *webViewStack, LuaRuntime *luaRuntime,
                KeymapEvaluator *keymapEvaluator);
  ~InputMediator();

  DELEGATE(webViewStack, openUrl, openUrl)
  DELEGATE(webViewStack, currentUrl, currentUrl)
  DELEGATE(webViewStack, next, nextWebView)
  DELEGATE(webViewStack, previous, previousWebView)
  DELEGATE(webViewStack, closeCurrent, closeCurrentWebView)
  DELEGATE(keymapEvaluator, evaluate, evaluateKeymap)

protected:
  void evaluateCommand(QString command);
  void addKeymap(QString mode, QString keyseq, std::function<void()> action);

private:
  WebViewStack *webViewStack;
  LuaRuntime *luaRuntime;
  KeymapEvaluator *keymapEvaluator;
};
