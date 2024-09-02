#include <lionm/lionm.h>

double lionm_voltage(double power, double open_circuit_voltage,
                     double internal_resistance) {
  double current =
      lionm_current(power, open_circuit_voltage, internal_resistance);
  return lionm_voltage_from_current(power, current);
}

double lionm_voltage_from_current(double power, double current) {
  return power / current;
}
