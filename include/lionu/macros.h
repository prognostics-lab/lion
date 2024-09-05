#pragma once

#include "log.h"
#include <gsl/gsl_errno.h>
#include <lion/status.h>

#define LION_CALL(x, msg)                                                      \
  if (x != LION_STATUS_SUCCESS) {                                              \
    log_error(msg);                                                            \
    return LION_STATUS_FAILURE;                                                \
  }

#define LION_CALLDF(x, msg)                                                    \
  if (x != LION_STATUS_SUCCESS) {                                              \
    log_error(msg);                                                            \
  }

#define LION_GSL_CALL(x, msg)                                                  \
  if (x != GSL_SUCCESS) {                                                      \
    log_error(msg);                                                            \
    log_error("GSL error [%d]", x);                                            \
    return LION_STATUS_FAILURE;                                                \
  }

#define LION_GSL_CALLDF(x, msg)                                                \
  if (x != GSL_SUCCESS) {                                                      \
    log_error(msg);                                                            \
    log_error("GSL error [%d]", x);                                            \
    return LION_STATUS_FAILURE;                                                \
  }

#define LION_VCALL(x, msg, ...)                                                \
  if (x != LION_STATUS_SUCCESS) {                                              \
    log_error(msg, ##__VA_ARGS__);                                             \
    return LION_STATUS_FAILURE;                                                \
  }

#define LION_VCALLDF(x, msg, ...)                                              \
  if (x != LION_STATUS_SUCCESS) {                                              \
    log_error(msg, ##__VA_ARGS__);                                             \
  }

#define LION_GSL_VCALL(x, msg, ...)                                            \
  if (x != GSL_SUCCESS) {                                                      \
    log_error(msg, ##__VA_ARGS__);                                             \
    log_error("GSL error [%d]", x);                                            \
    return LION_STATUS_FAILURE;                                                \
  }

#define LION_GSL_VCALLDF(x, msg, ...)                                          \
  if (x != GSL_SUCCESS) {                                                      \
    log_error(msg, ##__VA_ARGS__);                                             \
    log_error("GSL error [%d]", x);                                            \
    return LION_STATUS_FAILURE;                                                \
  }
