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

  void test_evaluate() {
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

  void test_queue_task() {
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

  void test_sanity_check_uv_timer() {
    context("when a 1 second timer is set");
    it("calls callback after 1 second") {
      auto lua = LuaRuntime::instance();
      QSignalSpy evaluationCompletedSpy(lua, &LuaRuntime::evaluationCompleted);

      lua->evaluate(R"(
        _G.was_timer_called = false;
        local timer = uv.new_timer();
        timer:start(1000, 0, function()
          _G.was_timer_called = true;
          timer:close()
        end)
      )");
      QVERIFY(evaluationCompletedSpy.wait());
      QVERIFY(NOT lua->evaluateSync("return _G.was_timer_called").toBool());

      QVERIFY(QTest::qWaitFor([&lua]() {
        return lua->evaluateSync("return _G.was_timer_called").toBool();
      }));
    }
  }

  void test_sanity_check_uv_spawn() {
    it("calls exit callback when process exists") {
      auto lua = LuaRuntime::instance();
      QSignalSpy evaluationCompletedSpy(lua, &LuaRuntime::evaluationCompleted);

      lua->evaluate(R"(
        _G.spawn_exit_code = -1;
        local handle, pid = uv.spawn('ls', {}, function(code)
          _G.spawn_exit_code = code;
        end)
      )");
      QVERIFY(evaluationCompletedSpy.wait());
      QCOMPARE(lua->evaluateSync("return _G.spawn_exit_code").toInt(), -1);

      QVERIFY(QTest::qWaitFor([&lua]() {
        return 0 == lua->evaluateSync("return _G.spawn_exit_code").toInt();
      }));
    }
  }
};

QTEST_REGISTER(LuaRuntimeSpec)
#include "LuaRuntimeSpec.moc"
