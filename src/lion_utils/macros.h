#pragma once

#include "vendor/log.h"

#include <gsl/gsl_errno.h>
#include <lion/status.h>
#include <lionu/macros.h>

#define LION_CALL_I(x, msg, ...)                                               \
  if (x != LION_STATUS_SUCCESS) {                                              \
    logi_error(msg, #__VA_ARGS__);                                             \
    return LION_STATUS_FAILURE;                                                \
  }

#define LION_CALLDF_I(x, msg, ...)                                             \
  if (x != LION_STATUS_SUCCESS) {                                              \
    logi_error(msg, #__VA_ARGS__);                                             \
  }

#define LION_GSL_CALL_I(x, msg, ...)                                           \
  if (x != GSL_SUCCESS) {                                                      \
    logi_error(msg, #__VA_ARGS__);                                             \
    logi_error("GSL error [%d]", x);                                           \
    return LION_STATUS_FAILURE;                                                \
  }

#define LION_GSL_CALLDF_I(x, msg, ...)                                         \
  if (x != GSL_SUCCESS) {                                                      \
    logi_error(msg, #__VA_ARGS__);                                             \
    logi_error("GSL error [%d]", x);                                           \
    return LION_STATUS_FAILURE;                                                \
  }
