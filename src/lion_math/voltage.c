#include <lionm/lionm.h>

double lion_voltage(double power, double open_circuit_voltage,
                    double internal_resistance) {
  double current =
      lion_current(power, open_circuit_voltage, internal_resistance);
  return lion_voltage_from_current(power, current);
}

double lion_voltage_from_current(double power, double current) {
  return power / current;
}
