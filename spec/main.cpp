#include "TestUtils.h"
#include <QtWidgets/qapplication.h>

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  return runAllTests();
}
