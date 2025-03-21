#pragma once

#include <QWidget>
#include <QtCore/qmap.h>
#include <QtCore/qnamespace.h>
#include <QtCore>
#include <functional>

#include "keymap/KeySeqParser.hpp"
#include "utils.hpp"

typedef std::function<void()> KeyAction;

struct KeyMap {
  KeySequence keySequence;
  KeyAction action;
};

enum KeyMode { Normal, Insert };

class KeymapEvaluator : public QObject {
  Q_OBJECT

public:
  KeymapEvaluator();

  void addKeymap(KeyMode mode, QString key, KeyAction action);
  bool evaluate(Qt::KeyboardModifiers modifiers, Qt::Key key);
  KeyMode modeFromString(QString modeString);

  DEFINE_SETTER(setCurrentMode, currentMode)
  DEFINE_GETTER(getCurrentMode, currentMode)

private:
  const QList<KeyMap> *currentModeKeys();
  bool isInsertableMode();

private:
  QMap<KeyMode, QList<KeyMap>> modalKeys;
  KeySeqParser keySeqParser;
  KeyMode currentMode = KeyMode::Normal;
  KeySequence activeKeySequence;
};
