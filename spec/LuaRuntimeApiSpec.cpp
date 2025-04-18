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
  void lua_push(lua_State *state) const override {
    lua_newtable(state);
    SET_FIELD("test_data", integer, num);
  }
};

// NOLINTBEGIN
class LuaRuntimeApiSpec : public QObject {
  Q_OBJECT

private slots:
  void beforeTestCase() { LuaRuntime::instance().start_event_loop(); }

  void cleanupTestCase() { LuaRuntime::instance().stop_event_loop(); }

  void test_web_event_add_event() {
    context("when events, patterns and callback are specified correctly");
    it("returns true and registers event") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua, &LuaRuntime::evaluation_completed);

      lua.evaluate(R"(
        return web.event.add_listener({ 'Hello', 'World' }, {
          patterns = { 'p1', 'p2' },
          callback = function() print("Called") end,
        });
      )");
      evaluation_completed_spy.wait();

      QCOMPARE(evaluation_completed_spy.count(), 1);
      QVariant result = evaluation_completed_spy.takeFirst().at(0);
      QCOMPARE(result, true);
    }

    context("when a single event is passed");
    it("returns true and registers event") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua, &LuaRuntime::evaluation_completed);

      lua.evaluate(R"(
        return web.event.add_listener('Hello', {
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
      QSignalSpy evaluation_completed_spy(&lua, &LuaRuntime::evaluation_completed);

      lua.evaluate(R"(
        return web.event.add_listener({ 'Hello', 'World' }, {
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
      QSignalSpy evaluation_completed_spy(&lua, &LuaRuntime::evaluation_completed);

      lua.evaluate(R"(
        return web.event.add_listener(nil, {
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
      QSignalSpy evaluation_completed_spy(&lua, &LuaRuntime::evaluation_completed);

      lua.evaluate(R"(
        return web.event.add_listener({'Ev'}, {
          patterns = { 'p1', 'p2' },
        });
      )");
      evaluation_completed_spy.wait();

      QCOMPARE(evaluation_completed_spy.count(), 1);
      QVariant result = evaluation_completed_spy.first().first();
      QCOMPARE(result, false);
    }
  }

  void test_web_event_dispatching() {
    context("when dispatching a registered event (without pattern)");
    it("calls the registered event handler") {
      auto &lua = LuaRuntime::instance();
      QSignalSpy evaluation_completed_spy(&lua, &LuaRuntime::evaluation_completed);
      lua.evaluate(R"(
        _G.event1_called = false;
        _G.event1_called_with = nil;
        web.event.add_listener('TestEvent1', {
          callback = function(opts)
            _G.event1_called = true
            _G.event1_called_with = opts
          end,
        });
        _G.event2_called = false;
        web.event.add_listener('TestEvent2', {
          callback = function(opts) _G.event2_called = true end,
        });
      )");
      evaluation_completed_spy.wait();

      TestEvent1 event(42);
      WindowActionRouter::instance().dispatch_event(&event);

      QVERIFY(wait_for_lua_to_be_true("return _G.event1_called"));
      QVERIFY(wait_for_lua_to_be_true("return _G.event1_called_with.test_data == 42"));
      QVERIFY(wait_for_lua_to_be_true("return not _G.event2_called"));
    }
  }
};

QTEST_REGISTER(LuaRuntimeApiSpec)
#include "LuaRuntimeApiSpec.moc"
// NOLINTEND
