#pragma once

#include <QAbstractListModel>
#include <QWidget>
#include <QtCore>

struct Buffer {
  QString url;
  QString title;
};

class TabsModel : public QAbstractListModel {
  Q_OBJECT

public:
  TabsModel(QObject *parent = nullptr);
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QHash<int, QByteArray> roleNames() const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

private:
  QList<Buffer> items;
};
