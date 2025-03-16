#include <QAbstractListModel>
#include <QWidget>
#include <QtCore>

#include "completion/TabsModel.hpp"

TabsModel::TabsModel(QList<Tab> tabs, QObject *parent)
    : QAbstractListModel(parent), items(tabs) {}

QVariant TabsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= items.size())
    return QVariant();

  const Tab &item = items.at(index.row());

  if (role == Qt::DisplayRole)
    return index.row();
  else if (role == Qt::UserRole)
    return item.title;
  else if (role == Qt::AccessibleDescriptionRole)
    return item.url + item.title;

  return QVariant();
}

QHash<int, QByteArray> TabsModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[Qt::DisplayRole] = "url";
  roles[Qt::UserRole] = "title";
  roles[Qt::AccessibleDescriptionRole] = "full";
  return roles;
}

int TabsModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return items.size();
}

int TabsModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 2; // url + title
}
