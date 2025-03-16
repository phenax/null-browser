#pragma once

#include <QtCore/qnamespace.h>
#include <QtCore>

struct KeyChord {
  Qt::KeyboardModifiers mod;
  Qt::Key key;
};
bool operator==(const KeyChord a, const KeyChord b);

class KeySeqParser {
public:
  KeySeqParser();

  QList<KeyChord> parse(QString keySequence);

private:
  Qt::Key parseKey(QString keyName);
};
