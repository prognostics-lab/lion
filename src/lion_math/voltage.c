#include "voltage.h"

#include "current.h"

#include <lion/lion.h>

double lion_voltage(double power, double open_circuit_voltage, double internal_resistance, lion_params_t *params) {
  double current = lion_current(power, open_circuit_voltage, internal_resistance, params);
  return lion_voltage_from_current(power, current, params);
}

double lion_voltage_from_current(double power, double current, lion_params_t *params) { return power / current; }
