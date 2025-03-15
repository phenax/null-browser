#include <QWidget>
#include <QtCore>

#include "completion/FuzzySearchProxyModel.hpp"

bool FuzzySearchProxyModel::filterAcceptsRow(
    int sourceRow, const QModelIndex &sourceParent) const {
  QAbstractItemModel *model = sourceModel();
  QString filterText = filterRegularExpression().pattern();

  for (int col = 0; col < model->columnCount(); ++col) {
    QModelIndex index = model->index(sourceRow, col, sourceParent);
    QString data = model->data(index, Qt::AccessibleDescriptionRole).toString();

    // TODO: Fuzzy eet up
    if (data.contains(filterText, Qt::CaseInsensitive)) {
      return true;
    }
  }
  return false;
}
