#include "testUtils.h"
#include <QtWidgets/qapplication.h>

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  return run_all_tests();
}
