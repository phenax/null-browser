#include <QtCore>
#include <uv.h>

#include "LuaRuntime.hpp"
#include "testUtils.h"

class LuaRuntimeSpec : public QObject {
  Q_OBJECT

private slots:
  void beforeTestCase() { LuaRuntime::instance()->startEventLoop(); }

  void cleanupTestCase() {
    LuaRuntime::instance()->stopEventLoop();
    uv_library_shutdown();
  }

  void testEvaluate() {
    context("when given an expression returning a number");
    it("emits evaluationCompleted with the result") {
      auto lua = LuaRuntime::instance();
      QSignalSpy evaluationCompletedSpy(lua, &LuaRuntime::evaluationCompleted);

      lua->evaluate("return 20 + 1 * 5");

      evaluationCompletedSpy.wait();
      QCOMPARE(evaluationCompletedSpy.count(), 1);
      QVariant result = evaluationCompletedSpy.takeFirst().at(0);
      QCOMPARE(result, 25);
    }

    context("when given an expression returning a string");
    it("emits evaluationCompleted with the result") {
      auto lua = LuaRuntime::instance();
      QSignalSpy evaluationCompletedSpy(lua, &LuaRuntime::evaluationCompleted);

      lua->evaluate("local name = 'world'; return 'hello ' .. name");

      evaluationCompletedSpy.wait();
      QCOMPARE(evaluationCompletedSpy.count(), 1);
      QVariant result = evaluationCompletedSpy.takeFirst().at(0);
      QCOMPARE(result, "hello world");
    }

    context("when given an expression returning a boolean");
    it("emits evaluationCompleted with the result") {
      auto lua = LuaRuntime::instance();
      QSignalSpy evaluationCompletedSpy(lua, &LuaRuntime::evaluationCompleted);

      lua->evaluate("local num = 5; return 5 == num");

      evaluationCompletedSpy.wait();
      QCOMPARE(evaluationCompletedSpy.count(), 1);
      QVariant result = evaluationCompletedSpy.takeFirst().at(0);
      QCOMPARE(result, true);
    }

    context("when given an expression returning nil");
    it("emits evaluationCompleted with the result") {
      auto lua = LuaRuntime::instance();
      QSignalSpy evaluationCompletedSpy(lua, &LuaRuntime::evaluationCompleted);

      lua->evaluate("return nil");

      QVERIFY(evaluationCompletedSpy.wait());
      QCOMPARE(evaluationCompletedSpy.count(), 1);
      QVariant result = evaluationCompletedSpy.takeFirst().at(0);
      QCOMPARE(result, 0);
    }
  }

  void testQueueTask() {
    context("when task is queued");
    it("evaluates task asynchronously") {
      auto lua = LuaRuntime::instance();
      bool wasTaskCalled = false;

      lua->queueTask([&wasTaskCalled]() { wasTaskCalled = true; });
      QVERIFY(NOT wasTaskCalled);

      QVERIFY(QTest::qWaitFor([&wasTaskCalled]() { return wasTaskCalled; }));
      QVERIFY(wasTaskCalled);
    }
  }

  void testSanityCheckUV() {
    context("when a 1 second timer is set");
    it("calls callback after 1 second") {
      auto lua = LuaRuntime::instance();
      QSignalSpy evaluationCompletedSpy(lua, &LuaRuntime::evaluationCompleted);

      lua->evaluate(R"(
        _G.wasTimerCalled = false;
        local timer = uv.new_timer();
        timer:start(1000, 0, function()
          _G.wasTimerCalled = true;
          timer:close()
        end)
      )");
      QVERIFY(evaluationCompletedSpy.wait());
      QVERIFY(NOT lua->evaluateSync("return _G.wasTimerCalled").toBool());

      QVERIFY(QTest::qWaitFor([&lua]() {
        return lua->evaluateSync("return _G.wasTimerCalled").toBool();
      }));
    }
  }
};

QTEST_REGISTER(LuaRuntimeSpec)
#include "LuaRuntimeSpec.moc"
