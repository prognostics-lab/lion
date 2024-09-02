#pragma once

#include "params.h"

double lion_voltage(double power, double open_circuit_voltage,
                    double internal_resistance, lion_params_t *params);
double lion_voltage_from_current(double power, double current,
                                 lion_params_t *params);
