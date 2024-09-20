#pragma once

#include <lion/status.h>

namespace lion {

enum Status {
  SUCCESS = LION_STATUS_SUCCESS,
  FAILURE = LION_STATUS_FAILURE,
  EXIT = LION_STATUS_EXIT,
};

}
