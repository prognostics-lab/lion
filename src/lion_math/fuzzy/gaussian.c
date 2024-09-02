#include <gsl/gsl_math.h>

#include <lionm/lionm.h>

double lion_mf_gaussian(double x, lion_mf_gaussian_params_t *params) {
  double num = 0.5 * gsl_pow_2(x - params->mean);
  double den = gsl_pow_2(params->sigma);
  return exp(-num / den);
}
