#include "test.h"

#include <stdio.h>
#include <string.h>

test_stats_t test_stats = {0};

static test_context_t current_context;
test_context_t *test_current = &current_context;

static void pass_assertion(void) {
  test_stats.assertions_run++;
  test_stats.assertions_passed++;
}

static void fail_assertion(void) {
  test_stats.assertions_run++;
  test_stats.assertions_failed++;

  if (test_current) {
    test_current->failed = 1;
  }
}

void test_fail(const char *file, int line, const char *message) {
  printf("FAIL [%s]\n"
         "  at %s:%d\n"
         "  %s\n",
         test_current->name, file, line, message);
}

void test_expect_true_impl(int value, const char *expr, const char *file,
                           int line) {
  if (value) {
    pass_assertion();
    return;
  }

  fail_assertion();

  printf("FAIL [%s]\n"
         "  at %s:%d\n"
         "  EXPECT_TRUE(%s)\n",
         test_current->name, file, line, expr);
}

void test_assert_true_impl(int value, const char *expr, const char *file,
                           int line) {
  if (value) {
    pass_assertion();
    return;
  }

  fail_assertion();

  printf("FAIL [%s]\n"
         "  at %s:%d\n"
         "  ASSERT_TRUE(%s)\n",
         test_current->name, file, line, expr);

  longjmp(test_current->abort_env, 1);
}

void test_expect_str_eq_impl(const char *expected, const char *actual,
                             const char *expected_text, const char *actual_text,
                             const char *file, int line) {
  if (expected && actual && strcmp(expected, actual) == 0) {
    pass_assertion();
    return;
  }

  fail_assertion();

  printf("FAIL [%s]\n"
         "  at %s:%d\n"
         "  EXPECT_STR_EQ(%s, %s)\n"
         "  expected: \"%s\"\n"
         "  actual:   \"%s\"\n",
         test_current->name, file, line, expected_text, actual_text,
         expected ? expected : "(null)", actual ? actual : "(null)");
}

void test_assert_str_eq_impl(const char *expected, const char *actual,
                             const char *expected_text, const char *actual_text,
                             const char *file, int line) {
  if (expected && actual && strcmp(expected, actual) == 0) {
    pass_assertion();
    return;
  }

  fail_assertion();

  printf("FAIL [%s]\n"
         "  at %s:%d\n"
         "  ASSERT_STR_EQ(%s, %s)\n"
         "  expected: \"%s\"\n"
         "  actual:   \"%s\"\n",
         test_current->name, file, line, expected_text, actual_text,
         expected ? expected : "(null)", actual ? actual : "(null)");

  longjmp(test_current->abort_env, 1);
}

void test_run_test(const char *name, void (*fn)(void)) {
  current_context.name = name;
  current_context.failed = 0;

  printf("RUN  %s\n", name);

  if (setjmp(current_context.abort_env) == 0) {
    fn();
  }

  test_stats.tests_run++;

  if (current_context.failed) {
    printf("FAIL %s\n\n", name);
    test_stats.tests_failed++;
  } else {
    printf("PASS %s\n\n", name);
    test_stats.tests_passed++;
  }
}

void test_print_summary(void) {
  printf("\n");
  printf("========== SUMMARY ==========\n");
  printf("Tests Run:        %d\n", test_stats.tests_run);
  printf("Tests Passed:     %d\n", test_stats.tests_passed);
  printf("Tests Failed:     %d\n", test_stats.tests_failed);
  printf("\n");
  printf("Assertions Run:   %d\n", test_stats.assertions_run);
  printf("Assertions Passed:%d\n", test_stats.assertions_passed);
  printf("Assertions Failed:%d\n", test_stats.assertions_failed);
}
