#ifndef TEST_H
#define TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <setjmp.h>

typedef struct {
  int tests_run;
  int tests_passed;
  int tests_failed;

  int assertions_run;
  int assertions_passed;
  int assertions_failed;
} test_stats_t;

typedef struct {
  const char *name;
  int failed;
  jmp_buf abort_env;
} test_context_t;

extern test_stats_t test_stats;
extern test_context_t *test_current;

/* Runner */

void test_run_test(const char *name, void (*fn)(void));

void test_print_summary(void);

/* Internal assertion functions */

void test_fail(const char *file, int line, const char *message);

void test_expect_true_impl(int value, const char *expr, const char *file,
                           int line);

void test_assert_true_impl(int value, const char *expr, const char *file,
                           int line);

void test_expect_str_eq_impl(const char *expected, const char *actual,
                             const char *expected_text, const char *actual_text,
                             const char *file, int line);

void test_assert_str_eq_impl(const char *expected, const char *actual,
                             const char *expected_text, const char *actual_text,
                             const char *file, int line);

/* Public API */

#define TEST(name) static void name(void)

#define EXPECT_TRUE(expr)                                                      \
  test_expect_true_impl(!!(expr), #expr, __FILE__, __LINE__)

#define ASSERT_TRUE(expr)                                                      \
  test_assert_true_impl(!!(expr), #expr, __FILE__, __LINE__)

#define EXPECT_STR_EQ(expected, actual)                                        \
  do {                                                                         \
    const char *_e = (expected);                                               \
    const char *_a = (actual);                                                 \
                                                                               \
    test_expect_str_eq_impl(_e, _a, #expected, #actual, __FILE__, __LINE__);   \
  } while (0)

#define ASSERT_STR_EQ(expected, actual)                                        \
  do {                                                                         \
    const char *_e = (expected);                                               \
    const char *_a = (actual);                                                 \
                                                                               \
    test_assert_str_eq_impl(_e, _a, #expected, #actual, __FILE__, __LINE__);   \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif
