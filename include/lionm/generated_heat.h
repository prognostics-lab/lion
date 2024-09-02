#pragma once

#include "params.h"

double lion_generated_heat(double current, double internal_temperature,
                           double internal_resistance, double soc,
                           lion_params_t *params);
