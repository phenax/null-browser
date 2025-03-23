#include "testUtils.h"
#include <QtCore/qnamespace.h>
#include <QtCore>

#include "keymap/KeySeqParser.hpp"

// NOLINTBEGIN
class KeySeqParserSpec : public QObject {
  Q_OBJECT

private slots:
  void test_parse() {
    context("when mix of upper/lower cases");
    it("parses keys ignoring the casing") {
      KeySeqParser parser;

      QList<KeyChord> keys = parser.parse("<C-tab><c-tAb><c-Tab>");

      QList<KeyChord> expected_keys = {
          {.mod = Qt::ControlModifier, .key = Qt::Key_Tab},
          {.mod = Qt::ControlModifier, .key = Qt::Key_Tab},
          {.mod = Qt::ControlModifier, .key = Qt::Key_Tab},
      };
      QCOMPARE(keys, expected_keys);
    }

    context("when input contains individual keys");
    it("parses the characters") {
      KeySeqParser parser;

      QList<KeyChord> keys = parser.parse("ab<c><s><sPace><tab><esC>z");

      QList<KeyChord> expected_keys = {
          {.mod = Qt::NoModifier, .key = Qt::Key_A},
          {.mod = Qt::NoModifier, .key = Qt::Key_B},
          {.mod = Qt::NoModifier, .key = Qt::Key_C},
          {.mod = Qt::NoModifier, .key = Qt::Key_S},
          {.mod = Qt::NoModifier, .key = Qt::Key_Space},
          {.mod = Qt::NoModifier, .key = Qt::Key_Tab},
          {.mod = Qt::NoModifier, .key = Qt::Key_Escape},
          {.mod = Qt::NoModifier, .key = Qt::Key_Z},
      };
      QCOMPARE(keys, expected_keys);
    }
  }
};

QTEST_REGISTER(KeySeqParserSpec)
#include "KeySeqParserSpec.moc"
// NOLINTEND
