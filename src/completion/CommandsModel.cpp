#include <QAbstractListModel>
#include <QWidget>
#include <QtCore>

#include "completion/CommandsModel.hpp"

CommandsModel::CommandsModel(QObject *parent) : QAbstractListModel(parent) {
  items = commands;
}

QVariant CommandsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= items.size())
    return QVariant();

  const Command &item = items.at(index.row());

  if (role == Qt::DisplayRole)
    return item.name;
  else if (role == Qt::UserRole)
    return item.description;
  else if (role == Qt::AccessibleDescriptionRole)
    return item.name + item.description;

  return QVariant();
}

QHash<int, QByteArray> CommandsModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[Qt::DisplayRole] = "name";
  roles[Qt::UserRole] = "description";
  roles[Qt::AccessibleDescriptionRole] = "full";
  return roles;
}

int CommandsModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return items.size();
}

int CommandsModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 2; // name + description
}
