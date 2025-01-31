#pragma once

#include <lion/params.h>

#define LION_SLV_DIMENSION 2

int lion_slv_system_continuous(double t, const double state[], double out[], void *inputs);

int lion_slv_jac_analytical(double t, const double state[], double *dfdy, double dfdt[], void *inputs);
int lion_slv_jac_2point(double t, const double state[], double *dfdy, double dfdt[], void *inputs);
