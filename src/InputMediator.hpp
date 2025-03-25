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
  InputMediator(WebViewStack *webview_stack, LuaRuntime *lua_runtime,
                KeymapEvaluator *keymap_evaluator);
  ~InputMediator() override;

  DELEGATE(webview_stack, open_url, open_url)
  DELEGATE(webview_stack, current_url, current_url)
  DELEGATE(webview_stack, next, next_webview)
  DELEGATE(webview_stack, previous, previous_webview)
  DELEGATE(webview_stack, close_current, close_current_webview)
  DELEGATE(keymap_evaluator, evaluate, evaluate_keymap)

protected:
  void add_keymap(const QString &mode_string, const QString &keyseq,
                  std::function<void()> action);

private:
  WebViewStack *webview_stack;
  LuaRuntime *lua_runtime;
  KeymapEvaluator *keymap_evaluator;
};
