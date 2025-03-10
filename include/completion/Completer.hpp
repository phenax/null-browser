#pragma once

#include <QCompleter>
#include <QWidget>
#include <QtCore>

class Completer : public QCompleter {
  Q_OBJECT

public:
  Completer(QObject *parent = nullptr);

  // bool eventFilter(QObject *watched, QEvent *event) override;
};
