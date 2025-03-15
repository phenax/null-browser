#include <QAbstractItemView>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLineEdit>
#include <QTreeView>
#include <QWidget>
#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qitemselectionmodel.h>
#include <QtCore/qnamespace.h>
#include <QtCore/qsortfilterproxymodel.h>
#include <QtCore>
#include <QtWidgets/qboxlayout.h>

#include "completion/Completer.hpp"
#include "completion/CompleterDelegate.hpp"

const char *completerStyles = R"(
  background-color: #111;
  color: #fff;
  border-radius: 0;
)";

Completer::Completer(QWidget *parentNode) : QWidget(parentNode) {
  layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  setLayout(layout);
  setStyleSheet(completerStyles);
  setContentsMargins(0, 0, 0, 0);

  view = new QTreeView();
  layout->addWidget(view, 0);
  viewDelegate = new CompleterDelegate(view);
  view->setItemDelegate(viewDelegate);
  view->setRootIsDecorated(false);
  view->setUniformRowHeights(true);
  view->header()->hide();
  view->setColumnWidth(0, 500);
  view->setModel(&proxyModel);
  view->setFocusPolicy(Qt::NoFocus);
  view->setSelectionMode(QAbstractItemView::SingleSelection);
  view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
  view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void Completer::acceptHighlighted() {
  auto index = proxyModel.index(viewDelegate->currentRow(), 0);
  auto text = proxyModel.data(index, Qt::DisplayRole).toString();
  setHighlightedRow(0);
  emit accepted(text);
}

void Completer::onTextChange(QString text) {
  setHighlightedRow(0);
  proxyModel.setFilterWildcard(text);
}

bool Completer::onKeyPressEvent(QKeyEvent *event) {
  auto combo = event->keyCombination();
  auto row = viewDelegate->currentRow();

  // If there are no matches, nothing to handle here
  if (proxyModel.rowCount() == 0)
    return false;

  if (combo.key() == Qt::Key_Up) {
    setHighlightedRow(row <= 0 ? proxyModel.rowCount() - 1 : row - 1);
    return true;
  }

  if (combo.key() == Qt::Key_Down) {
    setHighlightedRow((row + 1) % proxyModel.rowCount());
    return true;
  }

  if (combo.key() == Qt::Key_Tab) {
    acceptHighlighted();
    return true;
  }

  if (combo.key() == Qt::Key_Return) {
    acceptHighlighted();
    return false;
  }

  return false;
}

void Completer::setHighlightedRow(uint32_t row) {
  viewDelegate->setCurrentRow(row);
  view->update();
}
