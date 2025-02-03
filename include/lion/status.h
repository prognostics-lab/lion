/// @file
/// @brief Status of a lion call.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// TODO: Evaluate removal of runtime error detection on release builds

/// @addtogroup types
/// @{

/// Status of each `lion` call.
typedef enum lion_status {
  LION_STATUS_SUCCESS, ///< Success.
  LION_STATUS_FAILURE, ///< Failure.
  LION_STATUS_EXIT,    ///< Trigger exit.
} lion_status_t;

/// @}

#ifdef __cplusplus
}
#endif
