#pragma once

#include <QtCore>
#include <cstdint>

struct KeyChord {
  Qt::KeyboardModifiers mod;
  Qt::Key key;

  bool operator==(const KeyChord &chord2) const;
};

using KeySequence = QList<KeyChord>;

enum KeyMatchType : uint8_t {
  NoMatch,
  Match,
  Pending,
};

class KeySeqParser {
public:
  static KeyMatchType key_sequence_match(const KeySequence &target, const KeySequence &current);

  KeySeqParser() = default;
  KeySequence parse(QString key_sequence);

private:
  Qt::Key parse_key(const QString &key_name);
};
