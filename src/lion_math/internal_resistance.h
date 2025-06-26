#pragma once

#include <lion/params.h>

#ifdef __cplusplus
extern "C" {
#endif

double lion_resistance(double soc, double current, double soh, lion_params_t *params);

#ifdef __cplusplus
}
#endif
