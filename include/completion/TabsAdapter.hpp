#pragma once

#include <QWidget>
#include <QtCore>

#include "completion/Adapter.hpp"

class TabsAdapter : public Adapter {
  Q_OBJECT

public:
  TabsAdapter();
  ~TabsAdapter();
  Completer *completer() override;
  QString prompt() override;

private:
  Completer *completerInstance;
};
