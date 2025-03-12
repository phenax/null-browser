#include <QBoxLayout>
#include <QCompleter>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QStringListModel>
#include <QWidget>
#include <QWidgetAction>
#include <QWindow>
#include <QtCore>

#include "completion/CommandsAdapter.hpp"
#include "widgets/InputLine.hpp"

const char *rootStyles = R"(
  background-color: #000;
  color: #fff;
  border-radius: 0;
)";

const char *promptStyles = R"(
  background-color: #222;
  color: #fff;
  padding: 0 2px;
)";

InputLine::InputLine(QString defaultInput, QWidget *parentNode)
    : QWidget(parentNode) {
  setContentsMargins(0, 0, 0, 0);
  move(0, 0);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setStyleSheet(rootStyles);

  auto layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  auto cmdLineBox = new QWidget();
  layout->addWidget(cmdLineBox);
  auto cmdLineLayout = new QHBoxLayout();
  cmdLineBox->setLayout(cmdLineLayout);
  cmdLineBox->layout()->setContentsMargins(0, 0, 0, 0);
  cmdLineLayout->setSpacing(0);

  promptPrefix = new QLabel();
  promptPrefix->setStyleSheet(promptStyles);
  promptPrefix->setContentsMargins(0, 0, 0, 0);
  input = new QLineEdit(defaultInput);
  input->setFocusPolicy(Qt::StrongFocus);

  cmdLineLayout->addWidget(promptPrefix);
  cmdLineLayout->addWidget(input);
  setFixedHeight(input->sizeHint().height());

  setAdapter(new CommandsAdapter());
}

void InputLine::setAdapter(Adapter *adapter) {
  if (this->adapterInstance)
    delete this->adapterInstance;
  this->adapterInstance = adapter;
  promptPrefix->setText(adapter->prompt());
  input->setCompleter(adapter->completer());
}

void InputLine::emitSubmit() { emit submitted(getInputText()); }

void InputLine::keyPressEvent(QKeyEvent *event) {
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

void InputLine::setInputFocus(bool focussed) {
  if (focussed)
    input->setFocus(Qt::PopupFocusReason);
  else
    input->clearFocus();
}
