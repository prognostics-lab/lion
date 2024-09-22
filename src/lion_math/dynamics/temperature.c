#include <lion/lion.h>
#include <lion_utils/vendor/log.h>

#include "temperature.h"

double lion_internal_temperature_d(double internal_temperature, double heat,
                                   double ambient_temperature,
                                   lion_params_t *params) {
  double rt = params->temp.rin + params->temp.rout;
  double delta = ambient_temperature - internal_temperature;
  double term1 = delta / rt;

  double term2 = heat;

  double diff = (term1 + term2) / params->temp.cp;
  return diff;
}

double lion_surface_temperature(double internal_temperature,
                                double ambient_temperature,
                                lion_params_t *params) {
  double rt = params->temp.rin + params->temp.rout;

  double term1 = internal_temperature * params->temp.rout / rt;
  double term2 = ambient_temperature * params->temp.rin / rt;
  return term1 + term2;
}
