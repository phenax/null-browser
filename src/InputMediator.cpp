#include <QList>
#include <QWidget>
#include <QtCore>

#include "InputMediator.hpp"
#include "LuaRuntime.hpp"
#include "keymap/KeymapEvaluator.hpp"
#include "widgets/WebViewStack.hpp"

// TODO: Rename this
InputMediator::InputMediator(WebViewStack *web_view_stack,
                             LuaRuntime *lua_runtime,
                             KeymapEvaluator *keymap_evaluator)
    : web_view_stack(web_view_stack), lua_runtime(lua_runtime),
      keymap_evaluator(keymap_evaluator) {
  connect(lua_runtime, &LuaRuntime::url_opened, web_view_stack,
          &WebViewStack::open_url);
  connect(lua_runtime, &LuaRuntime::keymap_add_requested, this,
          &InputMediator::add_keymap);
}

void InputMediator::add_keymap(const QString &mode_string,
                               const QString &keyseq,
                               std::function<void()> action) {
  const KeyMode mode = keymap_evaluator->mode_from_string(mode_string);
  keymap_evaluator->add_keymap(mode, keyseq, std::move(action));
}

// void InputMediator::evaluate_command(QString command) {
// CommandParser parser;
// auto cmd = parser.parse(command);
//
// switch (cmd.command) {
// case CommandType::LuaEval:
//   lua_runtime->evaluate(cmd.argsString);
//   break;
// case CommandType::Open:
//   open_url(cmd.argsString, OpenType::OpenUrl);
//   break;
// case CommandType::TabOpen:
//   open_url(cmd.argsString, OpenType::OpenUrlInTab);
//   break;
// case CommandType::TabNext:
//   next_web_view();
//   break;
// case CommandType::TabPrev:
//   previous_web_view();
//   break;
// case CommandType::TabSelect:
//   web_view_stack->focus_web_view(cmd.argsString.toLong());
//   break;
// case CommandType::Noop:
//   break;
// }
// }

InputMediator::~InputMediator() { delete web_view_stack; }
