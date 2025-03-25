#include <QList>
#include <QWidget>
#include <QtCore>

#include "InputMediator.hpp"
#include "LuaRuntime.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "widgets/WebViewStack.hpp"

// TODO: Rename this
InputMediator::InputMediator(WebViewStack *webview_stack,
                             LuaRuntime *lua_runtime,
                             KeymapEvaluator *keymap_evaluator)
    : webview_stack(webview_stack), lua_runtime(lua_runtime),
      keymap_evaluator(keymap_evaluator) {
  connect(lua_runtime, &LuaRuntime::url_opened, webview_stack,
          &WebViewStack::open_url);
  connect(lua_runtime, &LuaRuntime::keymap_added, this,
          &InputMediator::add_keymap);
  connect(lua_runtime, &LuaRuntime::history_back_requested, webview_stack,
          &WebViewStack::webview_history_back);
  connect(lua_runtime, &LuaRuntime::history_forward_requested, webview_stack,
          &WebViewStack::webview_history_forward);
  connect(lua_runtime, &LuaRuntime::webview_closed, webview_stack,
          &WebViewStack::close);

  lua_runtime->set_current_tab_id_fetcher(
      [this]() { return this->webview_stack->current_webview_id(); });
}

void InputMediator::add_keymap(const QString &mode_string,
                               const QString &keyseq,
                               std::function<void()> action) {
  const KeyMode mode = keymap_evaluator->mode_from_string(mode_string);
  keymap_evaluator->add_keymap(mode, keyseq, std::move(action));
}

InputMediator::~InputMediator() { delete webview_stack; }
