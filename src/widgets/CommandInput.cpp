#include <QCompleter>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QWidget>
#include <QWidgetAction>
#include <QWindow>
#include <QtCore/qnamespace.h>
#include <QtCore/qstringlistmodel.h>
#include <QtWidgets/qboxlayout.h>

#include "completion/CommandsModel.hpp"
#include "completion/Completer.hpp"
#include "widgets/CommandInput.hpp"

const char *rootStyles = R"(
  background-color: #000;
  color: #fff;
  border-radius: 0;
)";

CommandInput::CommandInput(QString defaultInput, QWidget *parentNode)
    : QWidget(parentNode) {
  setContentsMargins(0, 0, 0, 0);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setStyleSheet(rootStyles);

  auto layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  auto listModel = new CommandsModel();
  auto completer = new Completer(this);
  completer->setModel(listModel);

  auto cmdLineBox = new QWidget();
  layout->addWidget(cmdLineBox);
  auto cmdLineLayout = new QHBoxLayout();
  cmdLineBox->setLayout(cmdLineLayout);
  cmdLineBox->layout()->setContentsMargins(0, 0, 0, 0);
  cmdLineLayout->setSpacing(0);

  auto promptPrefix = new QLabel(tr(":"));
  promptPrefix->setContentsMargins(0, 0, 0, 0);
  input = new QLineEdit(defaultInput);
  input->setFocusPolicy(Qt::StrongFocus);
  input->setCompleter(completer);
  // input->installEventFilter(completer);

  cmdLineLayout->addWidget(promptPrefix);
  cmdLineLayout->addWidget(input);

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
