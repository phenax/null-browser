#pragma once

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QtCore>

#include "completion/Adapter.hpp"

class InputLine : public QWidget {
  Q_OBJECT

public:
  InputLine(QString defaultInput = "", QWidget *parentNode = nullptr);
  void setInputFocus(bool focussed);
  bool isInputFocussed();
  QString getInputCommand();
  void setInputText(QString text);
  void setAdapter(Adapter *adapter);

signals:
  void submitted(QString command);
  void cancelled();

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void emitSubmit();

private:
  QBoxLayout *layout;
  QLineEdit *input;
  QLabel *promptPrefix;
  Adapter *adapter = nullptr;
};
