#include <QtCore>
#include <uv.h>

#include "LuaRuntime.hpp"
#include "WindowActionRouter.hpp"
#include "events.hpp"
#include "testUtils.h"

class TestEvent1 : public BrowserEvent {
public:
  int num;
  TestEvent1(int num) : num(num) { kind = "TestEvent1"; }
  void lua_push(lua_State *state) override {
    lua_newtable(state);
    SET_FIELD("test_data", integer, num);
  }
};

// NOLINTBEGIN
class LuaRuntimeSpec : public QObject {
  Q_OBJECT

private slots:
  void beforeTestCase() { LuaRuntime::instance().start_event_loop(); }

  void cleanupTestCase() {
    LuaRuntime::instance().stop_event_loop();
    uv_library_shutdown();
  }

  void test_evaluate() {
    context("when given an expression returning a number");
    it("emits evaluation_completed with the result") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua,
                                          &LuaRuntime::evaluation_completed);

      lua.evaluate("return 20 + 1 * 5");

      evaluation_completed_spy.wait();
      QCOMPARE(evaluation_completed_spy.count(), 1);
      QVariant result = evaluation_completed_spy.takeFirst().at(0);
      QCOMPARE(result, 25);
    }

    context("when given an expression returning a string");
    it("emits evaluation_completed with the result") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua,
                                          &LuaRuntime::evaluation_completed);

      lua.evaluate("local name = 'world'; return 'hello ' .. name");

      evaluation_completed_spy.wait();
      QCOMPARE(evaluation_completed_spy.count(), 1);
      QVariant result = evaluation_completed_spy.takeFirst().at(0);
      QCOMPARE(result, "hello world");
    }

    context("when given an expression returning a boolean");
    it("emits evaluation_completed with the result") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua,
                                          &LuaRuntime::evaluation_completed);

      lua.evaluate("local num = 5; return 5 == num");

      evaluation_completed_spy.wait();
      QCOMPARE(evaluation_completed_spy.count(), 1);
      QVariant result = evaluation_completed_spy.takeFirst().at(0);
      QCOMPARE(result, true);
    }

    context("when given an expression returning nil");
    it("emits evaluation_completed with the result") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua,
                                          &LuaRuntime::evaluation_completed);

      lua.evaluate("return nil");

      QVERIFY(evaluation_completed_spy.wait());
      QCOMPARE(evaluation_completed_spy.count(), 1);
      QVariant result = evaluation_completed_spy.takeFirst().at(0);
      QCOMPARE(result, 0);
    }
  }

  void test_queue_task() {
    context("when task is queued");
    it("evaluates task asynchronously") {
      auto &lua = LuaRuntime::instance();
      bool was_task_called = false;

      lua.queue_task([&was_task_called]() { was_task_called = true; });
      QVERIFY(NOT was_task_called);

      QVERIFY(
          QTest::qWaitFor([&was_task_called]() { return was_task_called; }));
    }
  }

  void test_sanity_check_uv_timer() {
    context("when a 1 second timer is set");
    it("calls callback after 1 second") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua,
                                          &LuaRuntime::evaluation_completed);

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
    it("calls exit callback when process exists") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua,
                                          &LuaRuntime::evaluation_completed);

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

  void test_internals_register_event() {
    context("when events, patterns and callback are specified correctly");
    it("returns true and registers event") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua,
                                          &LuaRuntime::evaluation_completed);

      lua.evaluate(R"(
        return __internals.register_event({
          events = { 'Hello', 'World' },
          patterns = { 'p1', 'p2' },
          callback = function() print("Called") end,
        });
      )");
      evaluation_completed_spy.wait();

      QCOMPARE(evaluation_completed_spy.count(), 1);
      QVariant result = evaluation_completed_spy.takeFirst().at(0);
      QCOMPARE(result, true);
    }

    context("when patterns is not passed");
    it("returns true and registers event") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua,
                                          &LuaRuntime::evaluation_completed);

      lua.evaluate(R"(
        return __internals.register_event({
          events = { 'Hello', 'World' },
          callback = function() print("Called") end,
        });
      )");
      evaluation_completed_spy.wait();

      QCOMPARE(evaluation_completed_spy.count(), 1);
      QVariant result = evaluation_completed_spy.takeFirst().at(0);
      QCOMPARE(result, true);
    }

    context("when events list is not passed");
    it("returns false and doesnt register event") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua,
                                          &LuaRuntime::evaluation_completed);

      lua.evaluate(R"(
        return __internals.register_event({
          patterns = { 'p1', 'p2' },
          callback = function() print("Called") end,
        });
      )");
      evaluation_completed_spy.wait();

      QCOMPARE(evaluation_completed_spy.count(), 1);
      QVariant result = evaluation_completed_spy.first().first();
      QCOMPARE(result, false);
    }

    context("when callback is not passed");
    it("returns false and doesnt register event") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua,
                                          &LuaRuntime::evaluation_completed);

      lua.evaluate(R"(
        return __internals.register_event({
          events = {'Ev'},
          patterns = { 'p1', 'p2' },
        });
      )");
      evaluation_completed_spy.wait();

      QCOMPARE(evaluation_completed_spy.count(), 1);
      QVariant result = evaluation_completed_spy.first().first();
      QCOMPARE(result, false);
    }
  }

  void test_internals_register_event_handler() {
    context("when dispatching a registered event (without pattern)");
    it("calls the registered event handler") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua,
                                          &LuaRuntime::evaluation_completed);
      lua.evaluate(R"(
        _G.event1_called = false;
        _G.event1_called_with = nil;
        __internals.register_event({
          events = { 'TestEvent1' },
          callback = function(opts)
            _G.event1_called = true
            _G.event1_called_with = opts
          end,
        });
        _G.event2_called = false;
        __internals.register_event({
          events = { 'TestEvent2' },
          callback = function(opts) _G.event2_called = true end,
        });
      )");
      evaluation_completed_spy.wait();

      TestEvent1 event(42);
      WindowActionRouter::instance().dispatch_event(event);

      QVERIFY(wait_for_lua_to_be_true("return _G.event1_called"));
      QVERIFY(wait_for_lua_to_be_true(
          "return _G.event1_called_with.test_data == 42"));
      QVERIFY(wait_for_lua_to_be_true("return not _G.event2_called"));
    }
  }
};

QTEST_REGISTER(LuaRuntimeSpec)
#include "LuaRuntimeSpec.moc"
// NOLINTEND
