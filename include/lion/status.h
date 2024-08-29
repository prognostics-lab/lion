#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum lion_status {
  LION_SUCCESS = 0,
  LION_FAILURE = 1,
};

typedef struct lion_status_msg {
  enum lion_status status;
  const char *msg;
} lion_status_t;

lion_status_t lion_status_new(enum lion_status status, const char *msg);

#define LION_STATUS_SUCCESS lion_status_new(LION_SUCCESS, NULL)
#define LION_STATUS_FAILURE(msg) lion_status_new(LION_FAILURE, msg)

#ifdef __cplusplus
}
#endif
