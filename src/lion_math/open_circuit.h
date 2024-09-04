#pragma once

#include <lion/params.h>

double lion_voc(double soc, lion_params_t *params);
double lion_voc_grad(double soc, lion_params_t *params);
