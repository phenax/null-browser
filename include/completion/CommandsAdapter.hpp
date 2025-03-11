#pragma once

#include <QWidget>
#include <QtCore>

#include "completion/Adapter.hpp"

class CommandsAdapter : public Adapter {
  Q_OBJECT

public:
  CommandsAdapter();
  ~CommandsAdapter();
  Completer *completer() override;
  QString prompt() override;

private:
  Completer *completerInstance;
};
