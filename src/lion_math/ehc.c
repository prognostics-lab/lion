#include <lionm/lionm.h>

#include <gsl/gsl_math.h>

double lion_ehc(double soc, lion_params_t *params) {
  double exp_num = soc - params->ehc.mu;
  double exp_den = 2.0 * gsl_pow_2(params->ehc.sigma);
  double exp_term = -exp_num / exp_den;
  double first_term =
      M_SQRT1_2 / (M_SQRTPI * params->ehc.sigma) * exp(exp_term);

  double second_term = params->ehc.lambda * exp(-params->ehc.kappa * soc);
  return params->ehc.a * (first_term - second_term) + params->ehc.b;
}
