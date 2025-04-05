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
  static KeyMatchType key_sequence_match(const KeySequence &target, const KeySequence &current) {
    for (int i = 0; i < target.length(); i++) {
      if (current.length() <= i)
        return KeyMatchType::Pending;

      if (target[i].key != current[i].key || !target[i].mod.testFlags(current[i].mod))
        return KeyMatchType::NoMatch;
    }

    return KeyMatchType::Match;
  }

  KeySeqParser() = default;
  KeySequence parse(QString key_sequence);

private:
  Qt::Key parse_key(const QString &key_name);
};
