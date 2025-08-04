#include <QtCore>
#include <qtestcase.h>
#include <uv.h>

#include "LuaRuntime.hpp"
#include "lua.h"
#include "testUtils.h"

// NOLINTBEGIN
class LuaRuntimeSpec : public QObject {
  Q_OBJECT

private slots:
  void beforeTestCase() { LuaRuntime::instance().start_event_loop(); }

  void cleanupTestCase() { LuaRuntime::instance().stop_event_loop(); }

  void test_evaluate() {
    describe("#evaluate");

    context("when given an expression returning a number");
    it("emits evaluation_completed with the result") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua, &LuaRuntime::evaluation_completed);

      lua.evaluate("return 20 + 1 * 5");

      evaluation_completed_spy.wait();
      QCOMPARE(evaluation_completed_spy.count(), 1);
      QVariant result = evaluation_completed_spy.takeFirst().at(0);
      QCOMPARE(result, 25);
    }

    context("when given an expression returning a string");
    it("emits evaluation_completed with the result") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua, &LuaRuntime::evaluation_completed);

      lua.evaluate("local name = 'world'; return 'hello ' .. name");

      evaluation_completed_spy.wait();
      QCOMPARE(evaluation_completed_spy.count(), 1);
      QVariant result = evaluation_completed_spy.takeFirst().at(0);
      QCOMPARE(result, "hello world");
    }

    context("when given an expression returning a boolean");
    it("emits evaluation_completed with the result") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua, &LuaRuntime::evaluation_completed);

      lua.evaluate("local num = 5; return 5 == num");

      evaluation_completed_spy.wait();
      QCOMPARE(evaluation_completed_spy.count(), 1);
      QVariant result = evaluation_completed_spy.takeFirst().at(0);
      QCOMPARE(result, true);
    }

    context("when given an expression returning nil");
    it("emits evaluation_completed with the result") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua, &LuaRuntime::evaluation_completed);

      lua.evaluate("return nil");

      QVERIFY(evaluation_completed_spy.wait());
      QCOMPARE(evaluation_completed_spy.count(), 1);
      QVariant result = evaluation_completed_spy.takeFirst().at(0);
      QCOMPARE(result, 0);
    }
  }

  void test_queue_task() {
    describe("#queue_task");

    context("when task is queued");
    it("evaluates task asynchronously") {
      auto &lua = LuaRuntime::instance();
      bool was_task_called = false;

      lua.queue_task([&was_task_called]() { was_task_called = true; });
      QVERIFY(NOT was_task_called);

      QVERIFY(QTest::qWaitFor([&was_task_called]() { return was_task_called; }));
    }
  }

  void test_sanity_check_uv_timer() {
    describe("uv timer");

    context("when a 1 second timer is set");
    it("calls callback after 1 second") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua, &LuaRuntime::evaluation_completed);

      lua.evaluate(R"(
        _G.was_timer_called = false;
        local timer = uv.new_timer();
        timer:start(1000, 0, function()
          _G.was_timer_called = true;
          timer:close()
        end)
      )");
      QVERIFY(evaluation_completed_spy.wait());

      QVERIFY(wait_for_lua_to_be_true("return _G.was_timer_called"));
    }
  }

  void test_sanity_check_uv_spawn() {
    describe("uv spawn");

    it("calls exit callback when process exists") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua, &LuaRuntime::evaluation_completed);

      lua.evaluate(R"(
        _G.spawn_exit_code = -1;
        local handle, pid = uv.spawn('ls', {}, function(code)
          _G.spawn_exit_code = code;
        end)
      )");
      QVERIFY(evaluation_completed_spy.wait());

      QVERIFY(wait_for_lua_to_be_true("return _G.spawn_exit_code == 0"));
    }
  }

  void test_push_qvariant() {
    describe("LuaRuntime::push_qvariant");

    context("when called with a QString");
    it("pushes lua string") {
      auto &lua = LuaRuntime::instance();
      auto *state = lua.get_state();

      LuaRuntime::push_qvariant(state, QString("hello"));

      QVERIFY(lua_isstring(state, 1));
      QCOMPARE(lua_tostring(state, 1), "hello");
      lua_pop(state, 1);
    }

    context("when called with a QVariant int");
    it("pushes lua number") {
      auto &lua = LuaRuntime::instance();
      auto *state = lua.get_state();

      LuaRuntime::push_qvariant(state, QVariant(42));

      QVERIFY(lua_isnumber(state, 1));
      QCOMPARE(lua_tointeger(state, 1), 42);
      lua_pop(state, 1);
    }

    context("when called with a QVariant double");
    it("pushes lua number") {
      auto &lua = LuaRuntime::instance();
      auto *state = lua.get_state();

      LuaRuntime::push_qvariant(state, QVariant(42.69));

      QVERIFY(lua_isnumber(state, 1));
      QCOMPARE(lua_tonumber(state, 1), 42.69);
      lua_pop(state, 1);
    }

    context("when called with a QVariant bool");
    it("pushes lua boolean") {
      auto &lua = LuaRuntime::instance();
      auto *state = lua.get_state();

      LuaRuntime::push_qvariant(state, QVariant(true));

      QVERIFY(lua_isboolean(state, 1));
      QCOMPARE(lua_toboolean(state, 1), true);
      lua_pop(state, 1);
    }
  }
};

QTEST_REGISTER(LuaRuntimeSpec)
#include "LuaRuntimeSpec.moc"
// NOLINTEND
