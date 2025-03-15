#include <QAbstractListModel>
#include <QWidget>
#include <QtCore/qnamespace.h>
#include <QtCore>

#include "completion/UrlModel.hpp"

UrlModel::UrlModel(QObject *parent) : QAbstractListModel(parent) {
  items = {
      {.url = "https://google.com", .description = "Google"},
      {.url = "https://duckduckgo.com", .description = "DuckDuckGo"},
      {.url = "https://ediblemonad.dev", .description = "EdibleMonad website"},
      {.url = "https://lite.duckduckgo.com", .description = "DDG lite"},
      {.url = "https://github.com/trending", .description = "Github trending"},
  };
}

QVariant UrlModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= items.size())
    return QVariant();

  const Url &item = items.at(index.row());

  if (role == Qt::DisplayRole)
    return item.url;
  else if (role == Qt::UserRole)
    return item.description;
  else if (role == Qt::AccessibleDescriptionRole)
    return item.url + " " + item.description;

  return QVariant();
}

QHash<int, QByteArray> UrlModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[Qt::DisplayRole] = "url";
  roles[Qt::UserRole] = "description";
  roles[Qt::AccessibleDescriptionRole] = "full";
  return roles;
}

int UrlModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return items.size();
}

int UrlModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 2; // url + description
}
