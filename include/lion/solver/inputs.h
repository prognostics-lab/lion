#pragma once

#include <lion/params.h>

typedef struct lion_slv_inputs {
  double *sys_inputs;
  lion_params_t *sys_params;
} lion_slv_inputs_t;
