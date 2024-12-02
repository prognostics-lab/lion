#pragma once

#include <lion/params.h>

#ifdef __cplusplus
extern "C" {
#endif

// TODO: Implement state of health

double lion_kappa(double internal_temperature, lion_params_t *params);
double lion_kappa_grad(double internal_temperature, lion_params_t *params);
double lion_soc_usable(double soc, double kappa, lion_params_t *params);
double lion_capacity_usable(double capacity, double kappa, lion_params_t *params);

#ifdef __cplusplus
}
#endif
