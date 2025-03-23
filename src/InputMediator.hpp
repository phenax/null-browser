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
  InputMediator(WebViewStack *web_view_stack, LuaRuntime *lua_runtime,
                KeymapEvaluator *keymap_evaluator);
  ~InputMediator() override;

  DELEGATE(web_view_stack, open_url, open_url)
  DELEGATE(web_view_stack, current_url, current_url)
  DELEGATE(web_view_stack, next, next_web_view)
  DELEGATE(web_view_stack, previous, previous_web_view)
  DELEGATE(web_view_stack, close_current, close_current_web_view)
  DELEGATE(keymap_evaluator, evaluate, evaluate_keymap)

protected:
  void add_keymap(const QString &mode_string, const QString &keyseq,
                  std::function<void()> action);

private:
  WebViewStack *web_view_stack;
  LuaRuntime *lua_runtime;
  KeymapEvaluator *keymap_evaluator;
};
