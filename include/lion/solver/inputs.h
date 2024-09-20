#pragma once

#include <lion/params.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lion_app_state lion_app_state_t;

typedef struct lion_slv_inputs {
  lion_app_state_t *sys_inputs;
  lion_params_t *sys_params;
} lion_slv_inputs_t;

#ifdef __cplusplus
}
#endif

