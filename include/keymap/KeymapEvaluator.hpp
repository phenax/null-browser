#pragma once

#include "keymap/KeySeqParser.hpp"
#include <QWidget>
#include <QtCore/qmap.h>
#include <QtCore/qnamespace.h>
#include <QtCore>
#include <algorithm>

struct KeyMap {
  KeySequence keySequence;
  QString action; // TODO: string for testing
};

class KeymapEvaluator : public QObject {
  Q_OBJECT

public:
  KeymapEvaluator();

  void addKeymap(QString mode, QString key, QString action) {
    if (!modalKeys.contains(mode))
      modalKeys.insert(mode, {});

    auto keySeq = keySeqParser.parse(key);
    // TODO: Add actions
    modalKeys[mode].append((KeyMap){.keySequence = keySeq, .action = action});
  }

  void evaluate(Qt::KeyboardModifiers modifiers, Qt::Key key) {
    if (key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Meta ||
        key == Qt::Key_Alt)
      return;

    auto keymaps = currentModeKeys();
    auto foundPendingMatches = false;

    activeKeySequence.append((KeyChord){.mod = modifiers, .key = key});

    for (auto &keymap : *keymaps) {
      auto matchType =
          KeySeqParser::keySequenceMatch(keymap.keySequence, activeKeySequence);

      if (matchType == KeyMatchType::Match) {
        qDebug() << "Matched!" << keymap.action;
        activeKeySequence.clear();
        return;
      } else if (matchType == KeyMatchType::Pending) {
        foundPendingMatches = true;
      }
    }

    if (!foundPendingMatches)
      activeKeySequence.clear();
  }

private:
  const QList<KeyMap> *currentModeKeys() {
    if (!modalKeys.contains(currentMode))
      return new QList<KeyMap>();

    return &modalKeys[currentMode];
  }

private:
  QMap<QString, QList<KeyMap>> modalKeys;
  KeySeqParser keySeqParser;
  QString currentMode = "default";

  KeySequence activeKeySequence;
};
