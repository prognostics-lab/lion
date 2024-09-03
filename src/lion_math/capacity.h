#pragma once

#include "params.h"

double lion_kappa(double internal_temperature, lion_params_t *params);
double lion_soc_usable(double soc, double kappa, lion_params_t *params);
double lion_capacity_usable(double capacity, double kappa,
                            lion_params_t *params);
