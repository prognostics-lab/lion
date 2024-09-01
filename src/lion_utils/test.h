#pragma once
#include "vendor/log.h"
#include <otto/status.h>
#include <string.h>

#define TEST_FAIL 1
#define TEST_PASS 0

/* Assert macros */

#define OTTO_ASSERT(cond)                                                      \
  if (!(cond))                                                                 \
  return OTTO_STATUS_FAILURE(NULL)
#define OTTO_ASSERT_FALSE(cond)                                                \
  if (cond)                                                                    \
  return OTTO_STATUS_FAILURE(NULL)
#define OTTO_ASSERT_EQI(v1, v2)                                                \
  if ((v1) != (v2)) {                                                          \
    log_debug("TEST_FAIL: Expected %i, found %i", v2, v1);                     \
    return OTTO_STATUS_FAILURE(NULL);                                          \
  }
#define OTTO_ASSERT_NEI(v1, v2)                                                \
  if ((v1) == (v2)) {                                                          \
    log_debug("TEST_FAIL: Expected different than %i, found %i", v2, v1);      \
    return OTTO_STATUS_FAILURE(NULL);                                          \
  }
#define OTTO_ASSERT_STREQ(v1, v2)                                              \
  if (strcmp((v1), (v2))) {                                                    \
    log_debug("TEST_FAIL: Expected %s, found %s", v2, v1);                     \
    return OTTO_STATUS_FAILURE(NULL);                                          \
  }
#define OTTO_ASSERT_STRNE(v1, v2)                                              \
  if (!strcmp((v1), (v2))) {                                                   \
    log_debug("TEST_FAIL: Expected different than %s, found %s", v2, v1);      \
    return OTTO_STATUS_FAILURE(NULL);                                          \
  }
#define OTTO_ASSERT_FAILS(x)                                                   \
  if (x.status != OTTO_FAILURE) {                                              \
    log_debug("TEST_FAIL: Expected failure, got success");                     \
    return OTTO_STATUS_FAILURE(NULL);                                          \
  }

/* Test utilities */

#define OTTO_CALL_TEST(test)                                                   \
  log_info("TEST: *" #test "*");                                               \
  if (test().status == TEST_FAIL) {                                            \
    log_error("Found failing test");                                           \
    return TEST_FAIL;                                                          \
  }
