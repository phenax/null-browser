#pragma once

#include <QtTest/QtTest>
#include <QtTest/qtestcase.h>
#include <cstdio>

#define NOT !

#define ANSI_BOLD "\x1b[1m"
#define COLOR_DESCRIBE ANSI_BOLD
#define COLOR_CONTEXT "\x1b[32m" ANSI_BOLD
#define COLOR_IT "\x1b[36m" ANSI_BOLD
#define COLOR_SKIP "\x1b[33m" ANSI_BOLD
#define ANSI_RESET "\x1b[0m"

#define describe(msg)                                                                              \
  printf(COLOR_DESCRIBE "%s" ANSI_RESET "\n", msg);                                                \
  fflush(stdout);

#define context(msg)                                                                               \
  printf("│ " COLOR_CONTEXT "%s" ANSI_RESET "\n", msg);                                            \
  fflush(stdout);

#define it(msg)                                                                                    \
  printf("└─⚪" ANSI_BOLD COLOR_IT "it " ANSI_RESET COLOR_IT "%s" ANSI_RESET "\n", msg);           \
  fflush(stdout);                                                                                  \
  if (1)

#define xit(msg)                                                                                   \
  printf("└─⚪" COLOR_SKIP "SKIPPED it %s" ANSI_RESET "\n", msg);                                  \
  fflush(stdout);                                                                                  \
  if (0)

#define STRINGIFY(x) #x

std::vector<std::function<QObject *()>> &get_qtest_registry();
int run_app_tests();
int run_lua_tests();

#define QTEST_REGISTER(klass)                                                                      \
  namespace {                                                                                      \
  const bool registered__##klass = []() {                                                          \
    get_qtest_registry().push_back([]() {                                                          \
      auto test = new (klass);                                                                     \
      test->setObjectName(#klass);                                                                 \
      return test;                                                                                 \
    });                                                                                            \
    return true;                                                                                   \
  }();                                                                                             \
  };

bool wait_for_lua_to_be_true(QString lua_code);
