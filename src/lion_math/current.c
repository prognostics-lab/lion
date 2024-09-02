#include <gsl/gsl_math.h>
#include <lionm/lionm.h>

double lion_current(double power, double open_circuit_voltage,
                    double internal_resistance, lion_params_t *params) {
  // Aliases for equations
  double p = power;
  double voc = open_circuit_voltage;
  double r = internal_resistance;

  double sqrt_term = gsl_pow_2(voc / (2.0 * r)) - p / r;
  return (voc / (2.0 * r)) - sqrt(sqrt_term);
}
