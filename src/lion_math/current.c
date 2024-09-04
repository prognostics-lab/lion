#include <gsl/gsl_math.h>
#include <lion/lion.h>

#include "current.h"

double lion_current(double power, double open_circuit_voltage,
                    double internal_resistance, lion_params_t *params) {
  // Aliases for equations
  double p = power;
  double voc = open_circuit_voltage;
  double r = internal_resistance;

  double sqrt_term = gsl_pow_2(voc / (2.0 * r)) - p / r;
  return (voc / (2.0 * r)) - sqrt(sqrt_term);
}

double lion_current_grad_voc(double power, double open_circuit_voltage,
                             double internal_resistance,
                             lion_params_t *params) {
  double p = power;
  double voc = open_circuit_voltage;
  double r = internal_resistance;

  double term1 = 1.0 / (2.0 * r);

  double term2_first_den = gsl_pow_2(voc / (2.0 * r)) - p / r;
  double term2_first = 1 / sqrt(term2_first_den);
  double term2_second = voc / (4 * gsl_pow_2(r));
  double term2 = term2_first * term2_second;
  return term1 - term2;
}

double lion_optimize_current(double power, double open_circuit_voltage,
                             double initial_guess, lion_params_t *params) {
  // The goal is to find the current I that solves the equation I = f(I)
  // where f is some known equation. The issue is that f might no be invertible
  // or it might have a non-analitic inverse, thus I is found by solving the
  // optimization problem
  //   min ||I - f(I)||^2
}
