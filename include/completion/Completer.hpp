#pragma once

#include <QCompleter>
#include <QHBoxLayout>
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
  Completer(QWidget *parentNode = nullptr);

  DELEGATE((&proxyModel), setSourceModel, setSourceModel)
  DELEGATE((&proxyModel), sourceModel, sourceModel)

signals:
  void accepted(QString text);

public slots:
  void onTextChange(QString text);
  bool onKeyPressEvent(QKeyEvent *event);

protected:
  void setHighlightedRow(uint32_t);
  void acceptHighlighted();

  DELEGATE(this, onKeyPressEvent, keyPressEvent)

private:
  QTreeView *view;
  FuzzySearchProxyModel proxyModel;
  CompleterDelegate *viewDelegate;
  QHBoxLayout *layout;
};
