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
#include <iostream>
#include <lua.hpp>

#include "CommandInput.hpp"

CommandInput::CommandInput(QString defaultInput, QWidget *parentNode)
    : QFrame(parentNode) {
  setContentsMargins(0, 0, 0, 0);
  // setFrameRect(QRect(0, 0, parentWidget()->width(), 50));
  setFixedWidth(parentWidget()->width());
  setFrameShadow(QFrame::Shadow::Raised);
  setFrameShape(QFrame::Box);
  setStyleSheet("background-color: #333; color: #fff;");

  auto layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  input = new QLineEdit(defaultInput, this);
  input->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(input);
}

void CommandInput::keyPressEvent(QKeyEvent *event) {
  auto combo = event->keyCombination();
  if (combo.key() == Qt::Key_Return) {
    emit submitted(input->text());
  }
}

bool CommandInput::isInputFocussed() { return input->hasFocus(); }

void CommandInput::setInputFocus(bool focussed) {
  if (focussed)
    input->setFocus();
  else
    input->clearFocus();
}
