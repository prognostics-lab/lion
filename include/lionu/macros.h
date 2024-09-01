#pragma once

#include "log.h"
#include <lion/status.h>

#define LION_CALL(x, msg, ...)                                                 \
  if (x != LION_STATUS_SUCCESS) {                                              \
    log_error(msg, #__VA_ARGS__);                                              \
    return LION_STATUS_FAILURE;                                                \
  }
