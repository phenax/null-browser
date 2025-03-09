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

class Cmd {
public:
  CommandType command;
  QStringList args;

  Cmd(CommandType command = Noop, QStringList args = QStringList())
      : command(command), args(args) {}
};

class CommandParser {
public:
  CommandParser();
  Cmd parse(QString command);

private:
  CommandType toCommandType(QString cmd);
};
