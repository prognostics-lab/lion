#pragma once

#include <lion/params.h>

#ifdef __cplusplus
extern "C" {
#endif

double lion_generated_heat(double current, double internal_temperature,
                           double internal_resistance, double ehc,
                           lion_params_t *params);

#ifdef __cplusplus
}
#endif
