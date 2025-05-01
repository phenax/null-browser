#include <QtCore>

#include "LuaRuntime.hpp"
#include "testUtils.h"

int run_lua_tests() {
  QString test_name = getenv("TEST_NAME");
  if (test_name.startsWith("lua ")) {
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

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  int exit_code_app = run_app_tests();
  int exit_code_lua = run_lua_tests();

  if (exit_code_app != 0)
    return exit_code_app;
  return exit_code_lua;
}
