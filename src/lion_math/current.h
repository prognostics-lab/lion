#pragma once

#include <lion/params.h>

double lion_current(double power, double open_circuit_voltage,
                    double internal_resistance, lion_params_t *params);
double lion_current_grad_voc(double power, double open_circuit_voltage,
                             double internal_resistance, lion_params_t *params);
double lion_current_optimize(double power, double open_circuit_voltage,
                             double initial_guess, lion_params_t *params);
