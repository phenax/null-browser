#include <QtTest/QtTest>
#include <QtTest/qtestcase.h>
#include <cstdio>

#include "LuaRuntime.hpp"

std::vector<std::function<QObject *()>> &get_qtest_registry() {
  static std::vector<std::function<QObject *()>> registry;
  return registry;
}

int run_app_tests() {
  int exit_code = 0;

  QString test_name = getenv("TEST_NAME");
  if (test_name.startsWith("lua:"))
    return 0;

  for (const auto &run_test : get_qtest_registry()) {
    auto *test = run_test();
    if (test_name.isEmpty() || test->objectName().contains(test_name))
      exit_code |= QTest::qExec(test);
    delete test;
  }

  get_qtest_registry().clear();
  return exit_code;
}

int run_lua_tests() {
  QString test_name = getenv("TEST_NAME");
  if (test_name.startsWith("lua:")) {
    test_name = test_name.remove(0, 4);
  } else if (!test_name.isEmpty())
    return 0;

  QDir dir("../spec/lua"); // TODO: relative to root instead of build/?
  QStringList spec_files = dir.entryList(QDir::Files);
  auto &lua = LuaRuntime::instance();

  lua.start_event_loop();
  lua.require_module("null-browser.test-utils");

  for (auto &file : spec_files) {
    auto is_spec = file.endsWith("_spec.lua");
    auto should_run_spec = test_name.isEmpty() || file.contains(test_name);
    if (is_spec && should_run_spec) {
      qDebug() << "Running suite: " << file;
      lua.load_file_sync(dir.filePath(file));
    }
  }

  lua.stop_event_loop();

  qDebug() << "Tests ran successfully";
  return 0;
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
