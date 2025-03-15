#pragma once

#include <QStyledItemDelegate>
#include <QWidget>
#include <QtCore/qabstractitemmodel.h>
#include <QtCore>
#include <cstdint>

class CompleterDelegate : public QStyledItemDelegate {
  Q_OBJECT

public:
  explicit CompleterDelegate(QObject *parent = nullptr);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;
  void setCurrentRow(uint32_t);
  uint32_t currentRow();

private:
  uint32_t row;
};
