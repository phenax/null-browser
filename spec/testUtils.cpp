#include <QtTest/QtTest>
#include <QtTest/qtestcase.h>
#include <cstdio>

std::vector<std::function<QObject *()>> &getQTestRegistry() {
  static std::vector<std::function<QObject *()>> registry;
  return registry;
}

int runAllTests() {
  int exitCode = 0;

  QString testName = getenv("TEST_NAME");

  for (const auto &runTest : getQTestRegistry()) {
    auto test = runTest();
    if (testName.isEmpty() || test->objectName().contains(testName))
      exitCode |= QTest::qExec(test);
    delete test;
  }

  getQTestRegistry().clear();
  return exitCode;
}
