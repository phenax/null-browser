#pragma once

#include <QtCore/qnamespace.h>
#include <QtCore>
#include <cmath>

struct KeyChord {
  Qt::KeyboardModifiers mod;
  Qt::Key key;
};
bool operator==(const KeyChord a, const KeyChord b);

typedef QList<KeyChord> KeySequence;

enum KeyMatchType {
  NoMatch,
  Match,
  Pending,
};

class KeySeqParser {
public:
  static const KeyMatchType keySequenceMatch(const KeySequence target,
                                             const KeySequence current) {
    for (int i = 0; i < target.length(); i++) {
      if (current.length() <= i)
        return KeyMatchType::Pending;

      if (target[i].key != current[i].key ||
          !target[i].mod.testFlags(current[i].mod))
        return KeyMatchType::NoMatch;
    }

    return KeyMatchType::Match;
  }

public:
  KeySeqParser();

  KeySequence parse(QString keySequence);

private:
  Qt::Key parseKey(QString keyName);
};
