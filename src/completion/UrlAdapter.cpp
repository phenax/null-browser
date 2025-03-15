#include <QWidget>
#include <QtCore>

#include "completion/Completer.hpp"
#include "completion/UrlAdapter.hpp"
#include "completion/UrlModel.hpp"

UrlAdapter::UrlAdapter() : Adapter() {
  completerInstance = new Completer();
  completerInstance->setSourceModel(new UrlModel(this));
}

Completer *UrlAdapter::completer() { return completerInstance; }

QString UrlAdapter::prompt() { return "[url]"; }

UrlAdapter::~UrlAdapter() {
  delete completerInstance->sourceModel();
  delete completerInstance;
}
