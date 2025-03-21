#include "testUtils.h"
#include <QtCore>
#include <atomic>
#include <uv.h>

#include "LuaRuntime.hpp"

class LuaRuntimeSpec : public QObject {
  Q_OBJECT

private slots:
  void cleanupTestCase() { uv_library_shutdown(); }

  void testSanityCheckBuiltins() {
    auto lua = LuaRuntime::instance();

    it("evaluates simple expression") {
      lua->startEventLoop();
      std::atomic<int> foobar = 2;

      lua->queueTask([&foobar]() { foobar = 10; });

      lua->evaluate(R"(
        print('Hello -- ');
        local timer = uv.new_timer();
        timer:start(1000, 0, function()
          print('inside timer 1')
          print('inside timer 2')
          print('inside timer 3')
          print('inside timer 4')
          print('inside timer 5')
          timer:close()
        end);
        print('-- end');
      )");
      lua->queueTask([]() { qDebug() << "---- 1"; });
      lua->queueTask([]() { qDebug() << "---- 2"; });
      lua->queueTask([]() { qDebug() << "---- 3"; });
      std::this_thread::sleep_for(std::chrono::seconds(2));
      // std::this_thread::sleep_for(std::chrono::milliseconds(20));
      lua->stopEventLoop();

      qDebug() << "foobar" << foobar;

      QCOMPARE(1, 1);
    }
  }

  void testSanityCheckBuiltins2() {
    auto lua = LuaRuntime::instance();

    it("evaluates again") {
      lua->startEventLoop();

      lua->queueTask([]() { qDebug() << "---- 5"; });
      lua->evaluate(R"(
        print('Hello -- ');
        local timer = uv.new_timer();
        timer:start(1000, 0, function()
          print('%%%%% blagb')
          timer:close()
        end);
        print('-- end');
      )");
      lua->queueTask([]() { qDebug() << "---- 5"; });
      // TODO: Impl
      std::this_thread::sleep_for(std::chrono::seconds(2));
      lua->stopEventLoop();

      QCOMPARE(1, 1);
    }
  }
};

QTEST_REGISTER(LuaRuntimeSpec)
#include "LuaRuntimeSpec.moc"
