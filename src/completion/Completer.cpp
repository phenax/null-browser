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

#include "completion/Completer.hpp"
#include "completion/CompleterDelegate.hpp"

const char *completerStyles = R"(
  background-color: #111;
  color: #fff;
  border-radius: 0;
  width: 100%;
)";

Completer::Completer() : QWidget() {
  view = new QTreeView(this);
  viewDelegate = new CompleterDelegate(view);
  view->setStyleSheet(completerStyles);
  view->setItemDelegate(viewDelegate);
  view->setRootIsDecorated(false);
  view->setUniformRowHeights(true);
  view->header()->hide();
  view->setColumnWidth(1, 500);
  view->setModel(&proxyModel);
  view->setFocusPolicy(Qt::NoFocus);
  view->setEditTriggers(QAbstractItemView::NoEditTriggers);
  view->setSelectionMode(QAbstractItemView::SingleSelection);
}

void Completer::setSourceModel(QAbstractItemModel *model) {
  proxyModel.setSourceModel(model);
}

void Completer::onTextChange(QString text) {
  proxyModel.setFilterWildcard(text);
}

void Completer::setHighlightedRow(uint32_t row) {
  viewDelegate->setCurrentRow(row);
  view->update();
}

void Completer::acceptHighlighted() {
  auto index = proxyModel.index(viewDelegate->currentRow(), 0);
  auto text = proxyModel.data(index, Qt::DisplayRole).toString();
  emit accepted(text);
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
