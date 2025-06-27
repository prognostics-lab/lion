#pragma once

#include <lion/params.h>

#ifdef __cplusplus
extern "C" {
#endif

double lion_soh_next(double soh, double soc_mean, double soc_max, double soc_min, double internal_temperature, lion_params_t *params);

#ifdef __cplusplus
}
#endif
