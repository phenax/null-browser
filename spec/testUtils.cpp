#include "LuaRuntime.hpp"
#include <QtTest/QtTest>
#include <QtTest/qtestcase.h>
#include <cstdio>

std::vector<std::function<QObject *()>> &get_qtest_registry() {
  static std::vector<std::function<QObject *()>> registry;
  return registry;
}

int run_app_tests() {
  int exit_code = 0;

  QString test_name = getenv("TEST_NAME");

  for (const auto &run_test : get_qtest_registry()) {
    auto *test = run_test();
    if (test_name.isEmpty() || test->objectName().contains(test_name))
      exit_code |= QTest::qExec(test);
    delete test;
  }

  get_qtest_registry().clear();
  return exit_code;
}

bool wait_for_lua_to_be_true(QString lua_code) {
  return QTest::qWaitFor([&lua_code]() {
    auto &lua = LuaRuntime::instance();
    QSignalSpy evaluation_completed_spy(&lua, &LuaRuntime::evaluation_completed);
    lua.evaluate(lua_code);
    evaluation_completed_spy.wait();
    return evaluation_completed_spy.first().first().toBool();
  });
}
