#pragma once

#include <QStyledItemDelegate>
#include <QWidget>
#include <QtCore>

class CompleterDelegate : public QStyledItemDelegate {
  Q_OBJECT

public:
  explicit CompleterDelegate(QObject *parent = nullptr);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;
};
