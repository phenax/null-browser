#include "testUtils.h"
#include <QtCore>

#include "keymap/KeymapEvaluator.hpp"

class KeymapEvaluatorSpec : public QObject {
  Q_OBJECT

private slots:
  void test_evaluate_single_key_chord() {
    context("when the key sequence is mapped");
    it("calls mapping") {
      int keymapWasCalled = false;
      KeymapEvaluator evaluator;
      evaluator.addKeymap(KeyMode::Normal, "<c-t>",
                          [&keymapWasCalled]() { keymapWasCalled = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);

      QVERIFY(keymapWasCalled);
    }

    context("when the key sequence is not mapped");
    it("does not call mapping") {
      int keymapWasCalled = false;
      KeymapEvaluator evaluator;
      evaluator.addKeymap(KeyMode::Normal, "<c-t>",
                          [&keymapWasCalled]() { keymapWasCalled = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_K);

      QVERIFY(NOT keymapWasCalled);
    }
  }

  void test_evaluate_multi_key_sequence() {
    context("when the full key sequence is mapped");
    it("calls mapping") {
      int keymapWasCalled = false;
      KeymapEvaluator evaluator;
      evaluator.addKeymap(KeyMode::Normal, "<c-t>a",
                          [&keymapWasCalled]() { keymapWasCalled = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);
      evaluator.evaluate(Qt::NoModifier, Qt::Key_A);

      QVERIFY(keymapWasCalled);
    }

    context("when only part of a mapped key sequence is entered");
    it("does not call mapping") {
      int keymapWasCalled = false;
      KeymapEvaluator evaluator;
      evaluator.addKeymap(KeyMode::Normal, "<c-t>a",
                          [&keymapWasCalled]() { keymapWasCalled = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);

      QVERIFY(NOT keymapWasCalled);
    }

    context("when the key sequence is not mapped");
    it("does not call mapping") {
      int keymapWasCalled = false;
      KeymapEvaluator evaluator;
      evaluator.addKeymap(KeyMode::Normal, "<c-t>a",
                          [&keymapWasCalled]() { keymapWasCalled = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_K);

      QVERIFY(NOT keymapWasCalled);
    }

    context("when the key sequence is not mapped");
    it("does not call mapping") {
      int keymapWasCalled = false;
      KeymapEvaluator evaluator;
      evaluator.addKeymap(KeyMode::Normal, "<c-t>a",
                          [&keymapWasCalled]() { keymapWasCalled = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);
      evaluator.evaluate(Qt::NoModifier, Qt::Key_B);

      QVERIFY(NOT keymapWasCalled);
    }

    context("when an incorrect sequence is entered before the correct one");
    it("calls mapping") {
      int keymapWasCalled = false;
      KeymapEvaluator evaluator;
      evaluator.addKeymap(KeyMode::Normal, "<c-t>a",
                          [&keymapWasCalled]() { keymapWasCalled = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);
      evaluator.evaluate(Qt::NoModifier, Qt::Key_B);
      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);
      evaluator.evaluate(Qt::NoModifier, Qt::Key_A);

      QVERIFY(keymapWasCalled);
    }

    // TODO: maybe fix this behavior
    context(
        "when partial mapped sequence is entered before an entire sequence");
    it("does not call mapping") {
      int keymapWasCalled = false;
      KeymapEvaluator evaluator;
      evaluator.addKeymap(KeyMode::Normal, "<c-t>a",
                          [&keymapWasCalled]() { keymapWasCalled = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);
      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);
      evaluator.evaluate(Qt::NoModifier, Qt::Key_A);

      QVERIFY(NOT keymapWasCalled);
    }
  }
};

QTEST_REGISTER(KeymapEvaluatorSpec)
#include "KeymapEvaluatorSpec.moc"
