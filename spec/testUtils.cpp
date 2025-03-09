#include <QtTest/QtTest>
#include <QtTest/qtestcase.h>
#include <cstdio>

std::vector<std::function<QObject *()>> &getQTestRegistry() {
  static std::vector<std::function<QObject *()>> registry;
  return registry;
}

int runAllTests() {
  int exitCode = 0;
  for (const auto &runTest : getQTestRegistry()) {
    auto test = runTest();
    // printf(":::: %s\n", test->objectName().toStdString().c_str());
    exitCode |= QTest::qExec(test);
    delete test;
  }
  getQTestRegistry().clear();
  return exitCode;
}
