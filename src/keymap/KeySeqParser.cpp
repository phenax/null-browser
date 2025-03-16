#include <QWidget>
#include <QtCore>
#include <algorithm>

#include "keymap/KeySeqParser.hpp"

bool operator==(const KeyChord a, const KeyChord b) {
  return a.mod == b.mod && a.key == b.key;
}

KeySeqParser::KeySeqParser() {}

QList<KeyChord> KeySeqParser::parse(QString keySequence) {
  QList<KeyChord> keyChords;
  KeyChord lastKey;

  // TODO: Refactor
  // TODO: Support <C-S-t>
  keySequence = keySequence.toLower();
  while (!keySequence.isEmpty()) {
    int skipCount = 1;
    if (keySequence.startsWith("<c-")) {
      int nextClosing = keySequence.indexOf('>');
      auto keyName = keySequence.sliced(3, nextClosing - 3);
      lastKey.mod = lastKey.mod | Qt::KeyboardModifier::ControlModifier;
      lastKey.key = parseKey(keyName);
      skipCount = nextClosing + 1;
    } else if (keySequence.startsWith("<s-")) {
      int nextClosing = keySequence.indexOf('>');
      auto keyName = keySequence.sliced(3, nextClosing - 3);
      lastKey.mod = lastKey.mod | Qt::KeyboardModifier::ShiftModifier;
      lastKey.key = parseKey(keyName);
      skipCount = nextClosing + 1;
    } else if (keySequence.startsWith("<")) {
      int nextClosing = keySequence.indexOf('>');
      auto keyName = keySequence.sliced(1, nextClosing - 1);
      lastKey.mod = Qt::KeyboardModifier::NoModifier;
      lastKey.key = parseKey(keyName);
      skipCount = nextClosing + 1;
    } else {
      auto keyName = keySequence.first(1);
      lastKey.mod = Qt::KeyboardModifier::NoModifier;
      lastKey.key = parseKey(keyName);
    }

    keySequence.slice(std::max(1, skipCount));
    keyChords.push_back(lastKey);
    lastKey = KeyChord();
  }

  return keyChords;
}

Qt::Key KeySeqParser::parseKey(QString keyName) {
  if (keyName.length() == 0)
    return Qt::Key_T; // TODO: tmp

  if (keyName.length() == 1) {
    char c = keyName.toStdString().at(0);
    return Qt::Key(Qt::Key_A + (c - 'a'));
  }

  if (keyName == "space")
    return Qt::Key_Space;

  if (keyName == "cr")
    return Qt::Key_Return;

  if (keyName == "esc")
    return Qt::Key_Escape;

  if (keyName == "bs")
    return Qt::Key_Backspace;

  if (keyName == "tab")
    return Qt::Key_Tab;

  return Qt::Key_T;
}
