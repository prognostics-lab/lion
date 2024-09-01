#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// TODO: Evaluate removal of runtime error detection on release builds

typedef enum lion_status {
  LION_STATUS_SUCCESS = 0,
  LION_STATUS_FAILURE = 1,
  LION_STATUS_EXIT    = 2,
} lion_status_t;

#ifdef __cplusplus
}
#endif
