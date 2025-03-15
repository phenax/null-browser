#pragma once

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QtCore>

#include "completion/Adapter.hpp"
#include "utils.hpp"

class InputLine : public QWidget {
  Q_OBJECT

public:
  InputLine(QString defaultInput = "", QWidget *parentNode = nullptr);
  void setInputFocus(bool focussed);
  bool isInputFocussed();
  void setAdapter(Adapter *adapter);

  DELEGATE(input, hasFocus, isInputFocussed)
  DELEGATE(input, setText, setInputText)
  DELEGATE(input, text, getInputText)
  DELEGATE(promptPrefix, text, prompt)

  // bool eventFilter(QObject *obj, QEvent *event) override;

signals:
  void submitted(QString text);
  void cancelled();

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void emitSubmit();

private:
  QLineEdit *input;
  QLabel *promptPrefix;
  Adapter *adapterInstance = nullptr;
};
