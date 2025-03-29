#include <QList>
#include <QWidget>
#include <QtCore>

#include "LuaRuntime.hpp"
#include "WindowMediator.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "widgets/WebViewStack.hpp"

WindowMediator::WindowMediator(WebViewStack *webview_stack,
                               LuaRuntime *lua_runtime,
                               KeymapEvaluator *keymap_evaluator)
    : webview_stack(webview_stack), lua_runtime(lua_runtime),
      keymap_evaluator(keymap_evaluator) {

  connect(this, &WindowMediator::keymap_added, this,
          &WindowMediator::add_keymap);

  connect(this, &WindowMediator::history_back_requested, webview_stack,
          &WebViewStack::webview_history_back);
  connect(this, &WindowMediator::history_forward_requested, webview_stack,
          &WebViewStack::webview_history_forward);

  connect(this, &WindowMediator::url_opened, webview_stack,
          &WebViewStack::open_url);
  connect(this, &WindowMediator::webview_closed, webview_stack,
          &WebViewStack::close);
  connect(this, &WindowMediator::webview_selected, webview_stack,
          &WebViewStack::focus_webview);
}

void WindowMediator::add_keymap(const QString &mode_string,
                                const QString &keyseq,
                                std::function<void()> action) {
  const KeyMode mode = keymap_evaluator->mode_from_string(mode_string);
  keymap_evaluator->add_keymap(mode, keyseq, std::move(action));
}

WindowMediator::~WindowMediator() { delete webview_stack; }
