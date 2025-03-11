#pragma once

#include <QWidget>
#include <QtCore>

#include "completion/Completer.hpp"

class Adapter : public QObject {
public:
  virtual Completer *completer() = 0;
  virtual QString prompt() = 0;
};
