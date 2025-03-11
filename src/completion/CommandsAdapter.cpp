#include <QWidget>
#include <QtCore>

#include "completion/CommandsAdapter.hpp"
#include "completion/CommandsModel.hpp"
#include "completion/Completer.hpp"

const QString cmdPrompt = "[exec]";

CommandsAdapter::CommandsAdapter() : Adapter() {
  completerInstance = new Completer(this);
  completerInstance->setModel(new CommandsModel(this));
}

Completer *CommandsAdapter::completer() { return completerInstance; }

QString CommandsAdapter::prompt() { return cmdPrompt; }

CommandsAdapter::~CommandsAdapter() {
  delete completerInstance->model();
  delete completerInstance;
}
