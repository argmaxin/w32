/* SPDX-License-Identifier: CC0-1.0 */
#include <stddef.h>

#if !defined __GNUC__ && !defined __clang__
#error "Use either GCC or Clang for testing."
#endif /* !__GNUC__ && !__clang__ */

#if !defined(TEST_H)
#define TEST_H

enum {
  TEST_OK = 0,
  TEST_WARN = 1,
  TEST_FAIL = 2
};

#define TEST_ASSERT(x)                                                         \
  do {                                                                         \
    if (!(x)) {                                                                \
      return TEST_FAIL;                                                        \
    }                                                                          \
  } while (0)

#define TEST_BAIL(x)                                                           \
  do {                                                                         \
    return ((x));                                                              \
  } while (0)

#define TEST_GROUP(name)                                                       \
  static struct tests *register_##name(void);                                  \
  __attribute__((constructor)) static void auto_##name(void) {                 \
    test_register_group(#name, register_##name);                               \
  }                                                                            \
  static struct tests *register_##name(void)

typedef long (*test_ft)(void);

struct tests;

typedef struct tests *(*test_group_reg_ft)(void);

void test_register_group(char const *name, test_group_reg_ft f);

struct tests *test_group_begin(char const *name);
int test_add(struct tests *t, char const *name, test_ft f);
void test_group_end(struct tests *t);

int test_run_all(int argc, char **argv);

#endif /* TEST_H */
