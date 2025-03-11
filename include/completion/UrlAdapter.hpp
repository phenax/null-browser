#pragma once

#include <QWidget>
#include <QtCore>

#include "completion/Adapter.hpp"

class UrlAdapter : public Adapter {
  Q_OBJECT

public:
  UrlAdapter();
  ~UrlAdapter();
  Completer *completer() override;
  QString prompt() override;

private:
  Completer *completerInstance;
};
