#pragma once

#include <QCompleter>
#include <QSortFilterProxyModel>
#include <QWidget>
#include <QtCore>
#include <QtWidgets/qtreeview.h>
#include <cstdint>

#include "completion/CompleterDelegate.hpp"
#include "completion/FuzzySearchProxyModel.hpp"
#include "utils.hpp"

class Completer : public QWidget {
  Q_OBJECT

public:
  Completer();
  void setSourceModel(QAbstractItemModel *model);

  DELEGATE((&proxyModel), sourceModel, sourceModel)

signals:
  void accepted(QString text);

public slots:
  void onTextChange(QString text);
  bool onKeyPressEvent(QKeyEvent *event);

protected:
  void keyPressEvent(QKeyEvent *event) override { onKeyPressEvent(event); }
  void setHighlightedRow(uint32_t);
  void acceptHighlighted();

private:
  QTreeView *view;
  FuzzySearchProxyModel proxyModel;
  CompleterDelegate *viewDelegate;
};
