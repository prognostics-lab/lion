#include <math.h>

#include <lion/lion.h>

#include "capacity.h"

double lion_kappa(double internal_temperature, lion_params_t *params) {
  double left = params->vft.k1 / (internal_temperature - params->vft.k2);
  double right = params->vft.k1 / (params->vft.tref - params->vft.k2);
  return exp(left - right);
}

double lion_kappa_grad(double internal_temperature, lion_params_t *params) {
  double coeff =
      params->vft.k1 / gsl_pow_2(internal_temperature - params->vft.k2);
  return -coeff * lion_kappa(internal_temperature, params);
}

double lion_soc_usable(double soc, double kappa, lion_params_t *params) {
  return 1.0 + (soc - 1.0) / kappa;
}

double lion_capacity_usable(double capacity, double kappa,
                            lion_params_t *params) {
  return kappa * capacity;
}
