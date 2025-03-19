#include "testUtils.h"
#include <QtWidgets/qapplication.h>

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  printf("foobar");

  return runAllTests();
}
