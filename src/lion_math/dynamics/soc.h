#pragma once

#include <lion/params.h>

#ifdef __cplusplus
extern "C" {
#endif

double lion_soc_d(double current, double usable_capacity,
                  lion_params_t *params);

#ifdef __cplusplus
}
#endif
