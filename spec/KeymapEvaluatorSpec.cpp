#include "testUtils.h"
#include <QtCore>

#include "keymap/KeymapEvaluator.hpp"

// NOLINTBEGIN
class KeymapEvaluatorSpec : public QObject {
  Q_OBJECT

private slots:
  void test_evaluate_single_key_chord() {
    context("when the key sequence is mapped");
    it("calls mapping") {
      auto keymap_was_called = false;
      KeymapEvaluator evaluator;
      evaluator.set_current_mode("n");
      evaluator.add_keymap("n", "<c-t>", [&keymap_was_called]() { keymap_was_called = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);

      QVERIFY(keymap_was_called);
    }

    context("when the key sequence is not mapped");
    it("does not call mapping") {
      auto keymap_was_called = false;
      KeymapEvaluator evaluator;
      evaluator.add_keymap("n", "<c-t>", [&keymap_was_called]() { keymap_was_called = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_K);

      QVERIFY(NOT keymap_was_called);
    }
  }

  void test_evaluate_multi_key_sequence() {
    context("when the full key sequence is mapped");
    it("calls mapping") {
      auto keymap_was_called = false;
      KeymapEvaluator evaluator;
      evaluator.set_current_mode("n");
      evaluator.add_keymap("n", "<c-t>a", [&keymap_was_called]() { keymap_was_called = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);
      evaluator.evaluate(Qt::NoModifier, Qt::Key_A);

      QVERIFY(keymap_was_called);
    }

    context("when only part of a mapped key sequence is entered");
    it("does not call mapping") {
      auto keymap_was_called = false;
      KeymapEvaluator evaluator;
      evaluator.set_current_mode("n");
      evaluator.add_keymap("n", "<c-t>a", [&keymap_was_called]() { keymap_was_called = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);

      QVERIFY(NOT keymap_was_called);
    }

    context("when the key sequence is not mapped");
    it("does not call mapping") {
      auto keymap_was_called = false;
      KeymapEvaluator evaluator;
      evaluator.set_current_mode("n");
      evaluator.add_keymap("n", "<c-t>a", [&keymap_was_called]() { keymap_was_called = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_K);

      QVERIFY(NOT keymap_was_called);
    }

    context("when the key sequence is not mapped");
    it("does not call mapping") {
      auto keymap_was_called = false;
      KeymapEvaluator evaluator;
      evaluator.set_current_mode("n");
      evaluator.add_keymap("n", "<c-t>a", [&keymap_was_called]() { keymap_was_called = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);
      evaluator.evaluate(Qt::NoModifier, Qt::Key_B);

      QVERIFY(NOT keymap_was_called);
    }

    context("when an incorrect sequence is entered before the correct one");
    it("calls mapping") {
      auto keymap_was_called = false;
      KeymapEvaluator evaluator;
      evaluator.set_current_mode("n");
      evaluator.add_keymap("n", "<c-t>a", [&keymap_was_called]() { keymap_was_called = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);
      evaluator.evaluate(Qt::NoModifier, Qt::Key_B);
      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);
      evaluator.evaluate(Qt::NoModifier, Qt::Key_A);

      QVERIFY(keymap_was_called);
    }

    // TODO: maybe fix this behavior
    context("when partial mapped sequence is entered before an entire sequence");
    it("does not call mapping") {
      auto keymap_was_called = false;
      KeymapEvaluator evaluator;
      evaluator.set_current_mode("n");
      evaluator.add_keymap("n", "<c-t>a", [&keymap_was_called]() { keymap_was_called = true; });

      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);
      evaluator.evaluate(Qt::ControlModifier, Qt::Key_T);
      evaluator.evaluate(Qt::NoModifier, Qt::Key_A);

      QVERIFY(NOT keymap_was_called);
    }
  }
};

QTEST_REGISTER(KeymapEvaluatorSpec)
#include "KeymapEvaluatorSpec.moc"
// NOLINTEND
