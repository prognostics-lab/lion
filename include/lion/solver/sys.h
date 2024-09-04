#pragma once

#include <lion/params.h>

int lion_slv_system(double t, double state[], double out[], void *inputs);
int lion_slv_jac(double t, double state[], double *dfdy, double dfdt[],
                 void *inputs);
