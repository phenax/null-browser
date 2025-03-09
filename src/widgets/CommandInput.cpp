#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QWindow>

#include "widgets/CommandInput.hpp"

CommandInput::CommandInput(QString defaultInput, QWidget *parentNode)
    : QWidget(parentNode) {
  setContentsMargins(0, 0, 0, 0);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setStyleSheet("background-color: #000; color: #fff; border-radius: 0;");

  auto layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  input = new QLineEdit(defaultInput, this);
  input->setFocusPolicy(Qt::StrongFocus);
  layout->addWidget(input);
  setFixedHeight(input->sizeHint().height());
}

void CommandInput::emitSubmit() { emit submitted(getInputCommand()); }

void CommandInput::keyPressEvent(QKeyEvent *event) {
  auto combo = event->keyCombination();
  auto ctrlL = combo.key() == Qt::Key_L &&
               combo.keyboardModifiers().testFlag(Qt::ControlModifier);
  auto esc = combo.key() == Qt::Key_Escape;
  auto enter = combo.key() == Qt::Key_Return || combo.key() == Qt::Key_Enter;

  if (esc || ctrlL)
    emit cancelled();
  else if (enter)
    emitSubmit();
}

void CommandInput::setInputText(QString text) { input->setText(text); }

bool CommandInput::isInputFocussed() { return input->hasFocus(); }

void CommandInput::setInputFocus(bool focussed) {
  if (focussed)
    input->setFocus(Qt::PopupFocusReason);
  else
    input->clearFocus();
}

QString CommandInput::getInputCommand() { return input->text(); }
