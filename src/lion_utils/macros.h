#pragma once

#include "vendor/log.h"
#include <lion/status.h>
#include <lionu/macros.h>

#define LION_CALL_I(x, msg, ...)                                               \
  if (x != LION_STATUS_SUCCESS) {                                              \
    logi_error(msg, #__VA_ARGS__);                                             \
    return LION_STATUS_FAILURE;                                                \
  }
