#include "testUtils.h"

#include "widgets/InputLine.hpp"

class InputLineSpec : public QObject {
  Q_OBJECT

private slots:
  void testWithInitialInput() {
    context("when initialized with some text");
    it("sets the initial input text") {
      InputLine inputWidget("Initial content", new QWidget());

      QCOMPARE(inputWidget.getInputCommand(), QString("Initial content"));
    }
  }

  void testInputUpdate() {
    context("when user types in input");
    it("updates input command") {
      InputLine inputWidget("Initial content", new QWidget());

      auto input = inputWidget.findChild<QLineEdit *>();
      QTest::keyClicks(input, " updated");

      QCOMPARE(inputWidget.getInputCommand(),
               QString("Initial content updated"));
    }

    context("when setInputText is called");
    it("sets input command") {
      InputLine inputWidget("Initial content", new QWidget());

      inputWidget.setInputText("New content");

      QCOMPARE(inputWidget.getInputCommand(), QString("New content"));
    }
  }

  void testSubmitSignalOnReturnKey() {
    context("when user hits return key");
    it("emits the submitted signal with the input command") {
      InputLine inputWidget("Initial content", new QWidget());
      QSignalSpy submitSignal(&inputWidget, &InputLine::submitted);

      auto input = inputWidget.findChild<QLineEdit *>();
      QTest::keyClicks(input, " updated");
      QTest::keyClick(&inputWidget, Qt::Key_Return);

      QCOMPARE(submitSignal.count(), 1);
      QCOMPARE(submitSignal.takeFirst().at(0).toString(),
               QString("Initial content updated"));
    }
  }

  void testCancelSignalOnEscapeKey() {
    context("when user hits escape key");
    it("emits 'cancelled' signal") {
      InputLine inputWidget("Initial content", new QWidget());
      QSignalSpy cancelSignal(&inputWidget, &InputLine::cancelled);

      auto input = inputWidget.findChild<QLineEdit *>();
      QTest::keyClick(&inputWidget, Qt::Key_Escape);

      QCOMPARE(cancelSignal.count(), 1);
    }

    context("when user hits ctrl+l key");
    it("emits 'cancelled' signal") {
      InputLine inputWidget("Initial content", new QWidget());
      QSignalSpy cancelSignal(&inputWidget, &InputLine::cancelled);

      auto input = inputWidget.findChild<QLineEdit *>();
      QTest::keyClick(&inputWidget, Qt::Key_L, Qt::ControlModifier);

      QCOMPARE(cancelSignal.count(), 1);
    }
  }

  void testSetFocus() {
    context("when setInputFocus is called with true");
    xit("focusses input field") {
      InputLine inputWidget("Initial content", new QWidget());

      inputWidget.setInputFocus(true);
      QApplication::processEvents();

      QVERIFY(inputWidget.isInputFocussed());
    }

    context("when setInputFocus is called with false");
    xit("unfocusses input field") {
      InputLine inputWidget("Initial content", new QWidget());

      inputWidget.setInputFocus(false);
      QApplication::processEvents();

      QVERIFY(!inputWidget.isInputFocussed());
    }
  }
};

QTEST_REGISTER(InputLineSpec)
#include "InputLineSpec.moc"
