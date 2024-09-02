#include <lionm/lionm.h>

#include <gsl/gsl_math.h>

double lion_generated_heat(double current, double internal_temperature,
                           double internal_resistance, double ehc,
                           lion_params_t *params) {
  double ohmic = internal_resistance * gsl_pow_2(current);
  double entropic = current * internal_temperature * ehc;
  return ohmic - entropic;
}
