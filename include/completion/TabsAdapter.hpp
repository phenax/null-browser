#pragma once

#include <QWidget>
#include <QtCore>

#include "completion/Adapter.hpp"
#include "completion/TabsModel.hpp"

class TabsAdapter : public Adapter {
  Q_OBJECT

public:
  TabsAdapter(QList<Tab> tabs);
  ~TabsAdapter();
  Completer *completer() override;
  QString prompt() override;

private:
  Completer *completerInstance;
};
