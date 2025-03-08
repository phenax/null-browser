#include <QtTest/QtTest>
#include <cstdio>

#define ANSI_BOLD "\x1b[1m"
#define COLOR_CONTEXT "\x1b[32m" ANSI_BOLD
#define COLOR_IT "\x1b[36m" ANSI_BOLD
#define COLOR_SKIP "\x1b[33m" ANSI_BOLD
#define ANSI_RESET "\x1b[0m"

#define context(msg) printf("⚪" COLOR_CONTEXT "%s" ANSI_RESET "\n", msg);

#define it(msg)                                                                \
  printf("    ⚪" ANSI_BOLD COLOR_IT "it " ANSI_RESET COLOR_IT "%s" ANSI_RESET \
         "\n",                                                                 \
         msg);                                                                 \
  if (1)

#define xit(msg)                                                               \
  printf("    ⚪" COLOR_SKIP "SKIPPED it %s" ANSI_RESET "\n", msg);            \
  if (0)
