#include <QWidget>
#include <QtCore>
#include <algorithm>
#include <cstdint>

#include "keymap/KeySeqParser.hpp"

bool KeyChord::operator==(const KeyChord &chord2) const {
  return this->mod == chord2.mod && this->key == chord2.key;
}

QList<KeyChord> KeySeqParser::parse(QString key_sequence) {
  QList<KeyChord> key_chords;
  KeyChord last_key;

  // TODO: Refactor
  // TODO: Support <C-S-t>
  key_sequence = key_sequence.toLower();
  while (!key_sequence.isEmpty()) {
    int skip_count = 1;
    uint16_t next_closing;
    if (key_sequence.startsWith("<c-")) {
      next_closing = key_sequence.indexOf('>');
      auto key_name = key_sequence.sliced(3, next_closing - 3);
      last_key.mod = last_key.mod | Qt::KeyboardModifier::ControlModifier;
      last_key.key = parse_key(key_name);
      skip_count = next_closing + 1;
    } else if (key_sequence.startsWith("<s-")) {
      next_closing = key_sequence.indexOf('>');
      auto key_name = key_sequence.sliced(3, next_closing - 3);
      last_key.mod = last_key.mod | Qt::KeyboardModifier::ShiftModifier;
      last_key.key = parse_key(key_name);
      skip_count = next_closing + 1;
    } else if (key_sequence.startsWith("<")) {
      next_closing = key_sequence.indexOf('>');
      auto key_name = key_sequence.sliced(1, next_closing - 1);
      last_key.mod = Qt::KeyboardModifier::NoModifier;
      last_key.key = parse_key(key_name);
      skip_count = next_closing + 1;
    } else {
      auto key_name = key_sequence.first(1);
      last_key.mod = Qt::KeyboardModifier::NoModifier;
      last_key.key = parse_key(key_name);
    }

    key_sequence.slice(std::max(1, skip_count));
    key_chords.push_back(last_key);
    last_key = KeyChord();
  }

  return key_chords;
}

Qt::Key KeySeqParser::parse_key(const QString &key_name) {
  if (key_name.length() == 0)
    return Qt::Key_T; // TODO: tmp

  if (key_name.length() == 1) {
    const char key_char = key_name.toStdString().at(0);
    if (key_char >= 'a' && key_char <= 'z')
      return Qt::Key(Qt::Key_A + (key_char - 'a'));
    if (key_char >= '0' && key_char <= '9')
      return Qt::Key(Qt::Key_0 + (key_char - '0'));
  }

  if (key_name == "space")
    return Qt::Key_Space;

  if (key_name == "cr")
    return Qt::Key_Return;

  if (key_name == "esc")
    return Qt::Key_Escape;

  if (key_name == "bs")
    return Qt::Key_Backspace;

  if (key_name == "tab")
    return Qt::Key_Tab;

  return Qt::Key_T;
}

KeyMatchType KeySeqParser::key_sequence_match(const KeySequence &target,
                                              const KeySequence &current) {
  for (int i = 0; i < target.length(); i++) {
    if (current.length() <= i)
      return KeyMatchType::Pending;

    if (target[i].key != current[i].key || !target[i].mod.testFlags(current[i].mod))
      return KeyMatchType::NoMatch;
  }

  return KeyMatchType::Match;
}
