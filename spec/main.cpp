#include <QtCore>

#include "schemes/schemes.hpp"
#include "testUtils.h"

int main(int argc, char **argv) {
  register_all_schemes();
  QApplication app(argc, argv);

  int exit_code_app = run_app_tests();
  int exit_code_lua = run_lua_tests();

  if (exit_code_app != 0)
    return exit_code_app;
  return exit_code_lua;
}
