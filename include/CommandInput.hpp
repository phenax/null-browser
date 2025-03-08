#pragma once

#include <QBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>

class CommandInput : public QFrame {
  Q_OBJECT

public:
  CommandInput(QString defaultInput = "", QWidget *parentNode = nullptr);
  void setInputFocus(bool focussed);
  bool isInputFocussed();
  QString getInputCommand();

signals:
  void submitted(QString command);

protected:
  void keyPressEvent(QKeyEvent *event) override;

private:
  QBoxLayout *layout;
  QLineEdit *input;
};
