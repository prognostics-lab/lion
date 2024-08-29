#pragma once

#include "log.h"
#include <lion/status.h>

#define LION_CALL(x, msg, ...)                                                 \
  if (x.status != LION_SUCCESS) {                                              \
    log_error(msg, #__VA_ARGS__);                                              \
    return LION_STATUS_FAILURE(msg);                                           \
  }
