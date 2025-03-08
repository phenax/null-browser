#pragma once

#include <QApplication>
#include <QDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QVBoxLayout>
#include <QWebEnginePage>
#include <QWebEngineView>
#include <QWidget>
#include <QWindow>
#include <lua.hpp>

class CommandInput : public QFrame {
  Q_OBJECT

public:
  CommandInput(QString defaultInput = "", QWidget *parentNode = nullptr);
  void setInputFocus(bool focussed);
  bool isInputFocussed();

signals:
  void submitted(QString command);

protected:
  void keyPressEvent(QKeyEvent *event) override;

private:
  QBoxLayout *layout;
  QLineEdit *input;
};
