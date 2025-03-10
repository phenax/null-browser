#pragma once

#include <QtCore/qcontainerfwd.h>
#include <QtCore>

enum CommandType {
  Noop,
  LuaEval,
  Open,
  TabOpen,
  TabNext,
  TabPrev,
};

struct Cmd {
  CommandType command;
  QString argsString;
  QString rawInput;
};

class CommandParser {
public:
  CommandParser();
  Cmd parse(QString command);

private:
  CommandType toCommandType(QString cmd);
};
