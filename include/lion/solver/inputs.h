#pragma once

#include <lion/params.h>

typedef struct lion_app_state lion_app_state_t;

typedef struct lion_slv_inputs {
  lion_app_state_t *sys_inputs;
  lion_params_t *sys_params;
} lion_slv_inputs_t;
