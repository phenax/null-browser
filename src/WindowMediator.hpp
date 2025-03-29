#pragma once

#include <QWidget>
#include <QtCore>

#include "LuaRuntime.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "utils.hpp"
#include "widgets/WebViewStack.hpp"

class WindowMediator : public QObject {
  Q_OBJECT

public:
  WindowMediator(WebViewStack *webview_stack, LuaRuntime *lua_runtime,
                 KeymapEvaluator *keymap_evaluator);
  ~WindowMediator() override;

  DELEGATE(keymap_evaluator, evaluate, evaluate_keymap)
  DELEGATE(webview_stack, has_webview, has_webview)

signals:
  void history_back_requested(WebViewId webview_id, qsizetype history_index);
  void history_forward_requested(WebViewId webview_id, qsizetype history_index);
  void keymap_added(QString mode, QString keyseq, std::function<void()>);
  void url_opened(QString url, OpenType open_type);
  void webview_closed(WebViewId webview_id);
  void webview_selected(WebViewId webview_id);

protected slots:
  void add_keymap(const QString &mode_string, const QString &keyseq,
                  std::function<void()> action);

private:
  WebViewStack *webview_stack;
  LuaRuntime *lua_runtime;
  KeymapEvaluator *keymap_evaluator;
};
