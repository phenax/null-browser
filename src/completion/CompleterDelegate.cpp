#include <QPainter>
#include <QWidget>
#include <QtCore/qlogging.h>
#include <QtCore/qnamespace.h>
#include <QtCore>

#include "completion/CompleterDelegate.hpp"

CompleterDelegate::CompleterDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void CompleterDelegate::paint(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const {
  painter->save();

  // Style
  if (option.state & QStyle::State_Selected) {
    painter->fillRect(option.rect, QColor("#aaa"));
    painter->setPen(Qt::black);
  } else {
    painter->fillRect(option.rect, QColor("#111"));
    painter->setPen(Qt::white);
  }

  // Draw text
  QString text;
  QRect rect;
  if (index.column() == 0) {
    text = index.data(Qt::DisplayRole).toString();
    rect = option.rect.adjusted(5, 0, 0, 0);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);
  } else if (index.column() == 1) {
    text = index.data(Qt::UserRole).toString();
    rect = option.rect.adjusted(0, 0, -5, 0);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);
  }

  painter->restore();
}
