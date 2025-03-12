#include <QtCore/qnamespace.h>
#include <QtCore>

#include "CommandParser.hpp"

CommandParser::CommandParser() {}

Cmd CommandParser::parse(QString input) {
  // TODO: simplify this to only parse the command
  auto parts = input.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

  if (parts.isEmpty())
    return {.command = Noop, .argsString = "", .rawInput = input};

  auto cmdStr = parts.first();
  auto cmd = toCommandType(cmdStr);
  auto rawArgs = input.slice(cmdStr.length()).trimmed();

  return {.command = cmd, .argsString = rawArgs, .rawInput = input};
}

CommandType CommandParser::toCommandType(QString cmd) {
  if (cmd == "lua")
    return LuaEval;
  if (cmd == "open")
    return Open;
  if (cmd == "tabopen")
    return TabOpen;
  if (cmd == "tn" || cmd == "tabnext")
    return TabNext;
  if (cmd == "tp" || cmd == "tabprev")
    return TabPrev;

  return Noop;
}
