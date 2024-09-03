#pragma once

#include <lion/params.h>

double lion_current(double power, double open_circuit_voltage,
                    double internal_resistance, lion_params_t *params);
