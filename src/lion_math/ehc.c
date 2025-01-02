#include "ehc.h"

#include <gsl/gsl_math.h>
#include <lion/lion.h>
#include <math.h>

double lion_ehc(double soc, lion_params_t *params) {
  double exp_num    = gsl_pow_2(soc - params->ehc.mu);
  double exp_den    = 2.0 * gsl_pow_2(params->ehc.sigma);
  double exp_term   = -exp_num / exp_den;
  double first_term = exp(exp_term) * M_SQRT1_2 / (M_SQRTPI * params->ehc.sigma);

  double second_term = params->ehc.l * exp(-params->ehc.kappa * soc);
  return params->ehc.a * (first_term - second_term) + params->ehc.b;
}
