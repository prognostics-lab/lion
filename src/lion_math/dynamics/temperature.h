#pragma once

#include <lion/params.h>

#ifdef __cplusplus
extern "C" {
#endif

double lion_internal_temperature_d(double internal_temperature, double heat, double ambient_temperature, lion_params_t *params);
double lion_surface_temperature(double internal_temperature, double ambient_temperature, lion_params_t *params);

#ifdef __cplusplus
}
#endif
