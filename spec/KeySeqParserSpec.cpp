#include "testUtils.h"
#include <QtCore/qnamespace.h>
#include <QtCore>

#include "keymap/KeySeqParser.hpp"

class KeySeqParserSpec : public QObject {
  Q_OBJECT

private slots:
  void testThings() {
    context("when mix of upper/lower cases");
    it("parses keys ignoring the casing") {
      KeySeqParser parser;

      QList<KeyChord> keys = parser.parse("<C-tab><c-tAb><c-Tab>");

      QList<KeyChord> expectedKeys = {
          {.mod = Qt::ControlModifier, .key = Qt::Key_Tab},
          {.mod = Qt::ControlModifier, .key = Qt::Key_Tab},
          {.mod = Qt::ControlModifier, .key = Qt::Key_Tab},
      };
      QCOMPARE(keys, expectedKeys);
    }

    context("when input contains individual keys");
    it("parses the characters") {
      KeySeqParser parser;

      QList<KeyChord> keys = parser.parse("ab<c><s><sPace><tab><esC>z");

      for (auto &k : keys) {
        qDebug() << k.key << k.mod;
      }

      QList<KeyChord> expectedKeys = {
          {.mod = Qt::NoModifier, .key = Qt::Key_A},
          {.mod = Qt::NoModifier, .key = Qt::Key_B},
          {.mod = Qt::NoModifier, .key = Qt::Key_C},
          {.mod = Qt::NoModifier, .key = Qt::Key_S},
          {.mod = Qt::NoModifier, .key = Qt::Key_Space},
          {.mod = Qt::NoModifier, .key = Qt::Key_Tab},
          {.mod = Qt::NoModifier, .key = Qt::Key_Escape},
          {.mod = Qt::NoModifier, .key = Qt::Key_Z},
      };
      QCOMPARE(keys, expectedKeys);
    }
  }
};

QTEST_REGISTER(KeySeqParserSpec)
#include "KeySeqParserSpec.moc"
