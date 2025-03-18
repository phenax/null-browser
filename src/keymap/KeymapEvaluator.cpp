#include <QWidget>
#include <QtCore>

#include "keymap/KeymapEvaluator.hpp"

KeymapEvaluator::KeymapEvaluator() : QObject() {}

// TODO: Clear mapping after some time

void KeymapEvaluator::addKeymap(KeyMode mode, QString key, KeyAction action) {
  if (!modalKeys.contains(mode))
    modalKeys.insert(mode, {});

  auto keySeq = keySeqParser.parse(key);
  modalKeys[mode].append((KeyMap){.keySequence = keySeq, .action = action});
}

bool KeymapEvaluator::evaluate(Qt::KeyboardModifiers modifiers, Qt::Key key) {
  if (key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Meta ||
      key == Qt::Key_Alt)
    return true;

  auto keymaps = currentModeKeys();
  auto foundPendingMatches = false;

  activeKeySequence.append((KeyChord){.mod = modifiers, .key = key});

  for (auto &keymap : *keymaps) {
    auto matchType =
        KeySeqParser::keySequenceMatch(keymap.keySequence, activeKeySequence);

    if (matchType == KeyMatchType::Match) {
      keymap.action();
      activeKeySequence.clear();
      return true;
    } else if (matchType == KeyMatchType::Pending) {
      foundPendingMatches = true;
    }
  }

  if (!foundPendingMatches)
    activeKeySequence.clear();

  if (isInsertableMode())
    return foundPendingMatches;

  return true;
}

bool KeymapEvaluator::isInsertableMode() {
  return currentMode == KeyMode::Insert;
}

const QList<KeyMap> *KeymapEvaluator::currentModeKeys() {
  if (!modalKeys.contains(currentMode))
    return new QList<KeyMap>();

  return &modalKeys[currentMode];
}
