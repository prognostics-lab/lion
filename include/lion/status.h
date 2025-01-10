#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// TODO: Evaluate removal of runtime error detection on release builds

typedef enum lion_status {
  LION_STATUS_SUCCESS,
  LION_STATUS_FAILURE,
  LION_STATUS_EXIT,
} lion_status_t;

#ifdef __cplusplus
}
#endif
