#include <QAbstractItemView>
#include <QHeaderView>
#include <QLineEdit>
#include <QTreeView>
#include <QWidget>
#include <QtCore>

#include "completion/Completer.hpp"
#include "completion/CompleterDelegate.hpp"

const char *completerStyles = R"(
  background-color: #111;
  color: #fff;
  border-radius: 0;
)";

Completer::Completer(QObject *parent) : QCompleter(parent) {
  setFilterMode(Qt::MatchContains);
  setCaseSensitivity(Qt::CaseInsensitive);
  setCompletionRole(Qt::DisplayRole);
  setCompletionMode(QCompleter::PopupCompletion);

  auto treeView = new QTreeView();
  setPopup(treeView);
  treeView->setStyleSheet(completerStyles);
  treeView->setItemDelegate(new CompleterDelegate(treeView));
  treeView->setRootIsDecorated(false);
  treeView->setUniformRowHeights(true);
  treeView->header()->hide();
  treeView->setColumnWidth(1, 300);
}

// bool Completer::eventFilter(QObject *watched, QEvent *event) {
//   auto isFocusIn = event->type() == QEvent::FocusIn;
//   auto isKeyRelease = event->type() == QEvent::KeyRelease;
//
//   QLineEdit *lineEdit = qobject_cast<QLineEdit *>(watched);
//   bool isInputEmpty = lineEdit && lineEdit->text().isEmpty();
//
//   if ((isFocusIn || isKeyRelease) && isInputEmpty)
//     complete();
//
//   return QCompleter::eventFilter(watched, event);
// }
