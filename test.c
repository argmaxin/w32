#define _POSIX_C_SOURCE 199309L

#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COL_RESET  "\033[0m"
#define COL_GREEN  "\033[32m"
#define COL_YELLOW "\033[33m"
#define COL_RED    "\033[31m"
#define COL_CYAN   "\033[36m"

struct test_res {
  char const *name;
  test_ft test;
  long code;
  struct test_res *next;
};

struct tests {
  char const *group_name;
  struct test_res *results;
  struct test_res *tail;
  size_t count;
  struct tests *next;
};

static struct tests *g_groups = NULL;
static struct tests *g_groups_tail = NULL;

static size_t g_total_tests = 0;
static size_t g_total_warns = 0;
static size_t g_total_fails = 0;

static long time_us(struct timespec a, struct timespec b) {
  return (b.tv_sec - a.tv_sec) * 1000000L + (b.tv_nsec - a.tv_nsec) / 1000L;
}

struct group_reg {
  char const *name;
  test_group_reg_ft fn;
  struct group_reg *next;
};

static struct group_reg *g_reg = NULL;
static struct group_reg *g_reg_tail = NULL;

void test_register_group(char const *name, test_group_reg_ft f) {
  struct group_reg *r = malloc(sizeof(*r));
  if (!r) {
    return;
  }

  r->name = name;
  r->fn = f;
  r->next = NULL;

  if (!g_reg) {
    g_reg = r;
  } else {
    g_reg_tail->next = r;
  }
  g_reg_tail = r;
}

struct tests *test_group_begin(char const *name) {
  struct tests *t = malloc(sizeof(*t));
  if (!t) {
    return NULL;
  }

  t->group_name = name;
  t->results = NULL;
  t->tail = NULL;
  t->count = 0;
  t->next = NULL;

  return t;
}

void test_group_end(struct tests *t) {
  if (!t) {
    return;
  }

  if (t == g_groups_tail || t->next != NULL) {
    return;
  }

  if (!g_groups) {
    g_groups = t;
  } else {
    g_groups_tail->next = t;
  }
  g_groups_tail = t;
}

int test_add(struct tests *t, char const *name, test_ft f) {
  if (!t) {
    return 1;
  }
  struct test_res *z = malloc(sizeof(*z));
  if (!z) {
    return 1;
  }

  *z = (struct test_res){.name = name, .test = f, .code = -1, .next = NULL};

  if (!t->results) {
    t->results = z;
  } else {
    t->tail->next = z;
  }

  t->tail = z;
  t->count++;

  return 0;
}

static void run_group(struct tests *t, char const *group_filter, char const *test_filter) {
  if (group_filter && !strstr(t->group_name, group_filter)) {
    return;
  }

  printf("\n" COL_CYAN " === Running group: %-20s ===\n" COL_RESET, t->group_name);
  struct test_res *z;
  for (z = t->results; z; z = z->next) {
    if (test_filter && !strstr(z->name, test_filter)) {
      continue;
    }
    
    struct timespec s, e;
    clock_gettime(CLOCK_MONOTONIC, &s);

    z->code = z->test();

    clock_gettime(CLOCK_MONOTONIC, &e);
    long us = time_us(s, e);

    char const *str;
    char const *col;

    switch (z->code) {
      case TEST_OK:
        str = "PASS";
        col = COL_GREEN;
        break;
      case TEST_WARN:
        str = "WARN";
        col = COL_YELLOW;
        g_total_warns++;
        break;
      default:
        str = "FAIL";
        col = COL_RED;
        g_total_fails++;
    }

    g_total_tests++;
    printf("%-30s %s%-4s%s (%ld us)\n", z->name, col, str, COL_RESET, us);
  }
}

int test_run_all(int argc, char **argv) {
  char *group_filter = NULL;
  char const *test_filter = NULL;

  if (argc > 1) {
    char *arg = argv[1];
    char *sep = strchr(arg, ':');

    if (sep) {
      size_t len = sep - arg;
      if (len > 0) {
        group_filter = malloc(len + 1);
        if (group_filter) {
          memcpy(group_filter, arg, len);
          group_filter[len] = '\0';
        }
      }
      if (*(sep + 1)) {
        test_filter = sep + 1;
      }
    } else {
      group_filter = arg;
    }
  }

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  struct group_reg *r;
  struct tests *t;

  for (r = g_reg; r; r = r->next) 
    r->fn();

  for (t = g_groups; t; t = t->next) run_group(t, group_filter, test_filter);

  clock_gettime(CLOCK_MONOTONIC, &end);
  long total_us = time_us(start, end);

  double pass_ratio = g_total_tests 
    ? (double)(g_total_tests - g_total_warns - g_total_fails) / g_total_tests 
    : 0.0;

  printf("\n+=+=+=+=+=+=+= SUMMARY =+=+=+=+=+=+=+=\n\n");
  printf("Total tests    : %-30zu\n", g_total_tests);
  printf("Pass %%         : %-30.2f\n", pass_ratio * 100);
  printf("Warnings       : %-30zu\n", g_total_warns);
  printf("Failures       : %-30zu\n", g_total_fails);
  printf("Total time(us) : %-30ld\n", total_us);
  printf("\n+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=\n");

  if (argc > 1 && strchr(argv[1], ':') && group_filter) 
    free(group_filter);
  
  struct tests *curr_g = g_groups;
  while (curr_g) {
    struct tests *next_g = curr_g->next;
    struct test_res *curr_r = curr_g->results;
    while (curr_r) {
      struct test_res *next_r = curr_r->next;
      free(curr_r);
      curr_r = next_r;
    }
    free(curr_g);
    curr_g = next_g;
  }

  struct group_reg *curr_reg = g_reg;
  while (curr_reg) {
    struct group_reg *next_reg = curr_reg->next;
    free(curr_reg);
    curr_reg = next_reg;
  }

  return g_total_fails ? 1 : 0;
}
