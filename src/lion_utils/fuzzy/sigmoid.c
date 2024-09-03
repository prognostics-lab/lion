#include <math.h>

#include <lionu/fuzzy.h>

double lion_mf_sigmoid(double x, lion_mf_sigmoid_params_t *params) {
  double exp_term = -params->a * (x - params->c);
  double denominator = 1 + exp(exp_term);
  return 1 / denominator;
}
