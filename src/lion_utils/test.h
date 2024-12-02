#pragma once
#include "vendor/log.h"

#include <lion/status.h>
#include <string.h>

#define TEST_FAIL 1
#define TEST_PASS 0

/* Assert macros */

#define LION_ASSERT(cond)                                                                                                                            \
  if (!(cond))                                                                                                                                       \
  return LION_STATUS_FAILURE
#define LION_ASSERT_FALSE(cond)                                                                                                                      \
  if (cond)                                                                                                                                          \
  return LION_STATUS_FAILURE
#define LION_ASSERT_EQI(v1, v2)                                                                                                                      \
  if ((v1) != (v2)) {                                                                                                                                \
    log_debug("TEST_FAIL: Expected %i, found %i", v2, v1);                                                                                           \
    return LION_STATUS_FAILURE;                                                                                                                      \
  }
#define LION_ASSERT_NEI(v1, v2)                                                                                                                      \
  if ((v1) == (v2)) {                                                                                                                                \
    log_debug("TEST_FAIL: Expected different than %i, found %i", v2, v1);                                                                            \
    return LION_STATUS_FAILURE;                                                                                                                      \
  }
#define LION_ASSERT_EQF(v1, v2)                                                                                                                      \
  if ((v1) != (v2)) {                                                                                                                                \
    log_debug("TEST_FAIL: Expected %f, found %f", v2, v1);                                                                                           \
    return LION_STATUS_FAILURE;                                                                                                                      \
  }
#define LION_ASSERT_NEF(v1, v2)                                                                                                                      \
  if ((v1) == (v2)) {                                                                                                                                \
    log_debug("TEST_FAIL: Expected different than %f, found %f", v2, v1);                                                                            \
    return LION_STATUS_FAILURE;                                                                                                                      \
  }
#define LION_ASSERT_STREQ(v1, v2)                                                                                                                    \
  if (strcmp((v1), (v2))) {                                                                                                                          \
    log_debug("TEST_FAIL: Expected %s, found %s", v2, v1);                                                                                           \
    return LION_STATUS_FAILURE;                                                                                                                      \
  }
#define LION_ASSERT_STRNE(v1, v2)                                                                                                                    \
  if (!strcmp((v1), (v2))) {                                                                                                                         \
    log_debug("TEST_FAIL: Expected different than %s, found %s", v2, v1);                                                                            \
    return LION_STATUS_FAILURE;                                                                                                                      \
  }
#define LION_ASSERT_FAILS(x)                                                                                                                         \
  if (x != LION_STATUS_FAILURE) {                                                                                                                    \
    log_debug("TEST_FAIL: Expected failure, got success");                                                                                           \
    return LION_STATUS_FAILURE;                                                                                                                      \
  }

/* Test utilities */

#define LION_CALL_TEST(app, test)                                                                                                                    \
  log_info("TEST: *" #test "*");                                                                                                                     \
  if (test(app) == TEST_FAIL) {                                                                                                                      \
    log_error("Found failing test");                                                                                                                 \
    return TEST_FAIL;                                                                                                                                \
  }
