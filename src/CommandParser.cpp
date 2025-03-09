#include <QtCore/qnamespace.h>
#include <QtCore>

#include "CommandParser.hpp"

CommandParser::CommandParser() {}

Cmd CommandParser::parse(QString input) {
  auto parts = input.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

  if (parts.isEmpty())
    return Cmd();

  auto cmd = toCommandType(parts.first());
  parts.removeFirst();
  return Cmd(cmd, parts);
}

CommandType CommandParser::toCommandType(QString cmd) {
  if (cmd == "lua")
    return LuaEval;
  if (cmd == "open")
    return Open;
  if (cmd == "tabopen")
    return TabOpen;
  if (cmd == "bn" || cmd == "bnext")
    return TabNext;
  if (cmd == "bp" || cmd == "bprevious")
    return TabPrev;

  return Noop;
}
