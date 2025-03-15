#pragma once

#include <QWidget>
#include <QtCore>

class FuzzySearchProxyModel : public QSortFilterProxyModel {
  Q_OBJECT

public:
  using QSortFilterProxyModel::QSortFilterProxyModel;

protected:
  bool filterAcceptsRow(int sourceRow,
                        const QModelIndex &sourceParent) const override;
};
