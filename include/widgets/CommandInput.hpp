#pragma once

#include <QBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>

class CommandInput : public QWidget {
  Q_OBJECT

public:
  CommandInput(QString defaultInput = "", QWidget *parentNode = nullptr);
  void setInputFocus(bool focussed);
  bool isInputFocussed();
  QString getInputCommand();
  void setInputText(QString text);

signals:
  void submitted(QString command);
  void cancelled();

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void emitSubmit();

private:
  QBoxLayout *layout;
  QLineEdit *input;
};
