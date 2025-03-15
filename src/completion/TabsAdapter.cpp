#include <QWidget>
#include <QtCore>

#include "completion/Completer.hpp"
#include "completion/TabsAdapter.hpp"
#include "completion/TabsModel.hpp"

TabsAdapter::TabsAdapter() : Adapter() {
  completerInstance = new Completer();
  completerInstance->setSourceModel(new TabsModel(this));
}

Completer *TabsAdapter::completer() { return completerInstance; }

QString TabsAdapter::prompt() { return "[tabs]"; }

TabsAdapter::~TabsAdapter() {
  delete completerInstance->sourceModel();
  delete completerInstance;
}
