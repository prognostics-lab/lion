#include <gsl/gsl_math.h>
#include <math.h>

#include <lion/lion.h>

#include "generated_heat.h"

double lion_generated_heat(double current, double internal_temperature,
                           double internal_resistance, double ehc,
                           lion_params_t *params) {
  double ohmic = internal_resistance * gsl_pow_2(current);
  double entropic = current * internal_temperature * ehc;
  double qgen = ohmic - entropic;
  return (qgen > 0.0) ? qgen : 0.0;
  // return ohmic - entropic;
}
