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
#include <QtCore/qnamespace.h>
#include <QtCore>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qwidget.h>

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
  padding: 0 4px;
)";

const char *inputStyles = R"(
  color: #fff;
  padding: 0 4px;
)";

InputLine::InputLine(QString defaultInput, QWidget *parentNode)
    : QWidget(parentNode) {
  setContentsMargins(0, 0, 0, 0);
  setStyleSheet(rootStyles);

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  setLayout(layout);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

  auto cmdLineBox = new QWidget();
  auto cmdLineLayout = new QHBoxLayout();
  cmdLineLayout->setContentsMargins(0, 0, 0, 0);
  cmdLineLayout->setSpacing(0);
  cmdLineBox->setLayout(cmdLineLayout);

  promptPrefix = new QLabel();
  promptPrefix->setStyleSheet(promptStyles);
  promptPrefix->setContentsMargins(0, 0, 0, 0);
  input = new QLineEdit(defaultInput);
  input->setStyleSheet(inputStyles);
  input->setFocusPolicy(Qt::StrongFocus);
  input->setContentsMargins(0, 0, 0, 0);

  cmdLineLayout->addWidget(promptPrefix);
  cmdLineLayout->addWidget(input, 1);
  cmdLineBox->setFixedHeight(input->sizeHint().height());

  layout->addWidget(cmdLineBox, 0);

  setAdapter(new CommandsAdapter());
}

void InputLine::setAdapter(Adapter *adapter) {
  if (this->adapterInstance) {
    layout->removeWidget(this->adapterInstance->completer());
    delete this->adapterInstance;
  }
  this->adapterInstance = adapter;
  promptPrefix->setText(adapter->prompt());

  auto completer = adapter->completer();
  layout->insertWidget(1, completer, 0, Qt::AlignmentFlag::AlignTop);

  connect(input, &QLineEdit::textChanged, completer, &Completer::onTextChange);
  connect(completer, &Completer::accepted, input, &QLineEdit::setText);
}

void InputLine::emitSubmit() { emit submitted(getInputText()); }

void InputLine::keyPressEvent(QKeyEvent *event) {
  auto combo = event->keyCombination();

  // Forward events to completer first
  bool shouldStop = adapterInstance->completer()->onKeyPressEvent(event);
  if (shouldStop)
    return;

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
