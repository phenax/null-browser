#pragma once

#include <QAbstractListModel>
#include <QWidget>
#include <QtCore>

struct Url {
  QString url;
  QString description;
};

class UrlModel : public QAbstractListModel {
  Q_OBJECT

public:
  UrlModel(QObject *parent = nullptr);
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QHash<int, QByteArray> roleNames() const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

private:
  QList<Url> items;
};
