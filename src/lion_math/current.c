#include "current.h"

#include "internal_resistance.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <lion/lion.h>
#include <lion_utils/vendor/log.h>
#include <math.h>

double lion_current(double power, double open_circuit_voltage, double internal_resistance, lion_params_t *params) {
  // Aliases for equations
  double p   = power;
  double voc = open_circuit_voltage;
  double r   = internal_resistance;

  double discriminant = gsl_pow_2(voc / (2.0 * r)) - p / r;
  if (discriminant < 0.0) {
    logi_warn("Found negative discriminant (d=%f)", discriminant);
  }
  return (voc / (2.0 * r)) - sqrt(discriminant);
}

double lion_current_grad_voc(double power, double open_circuit_voltage, double internal_resistance, lion_params_t *params) {
  double p   = power;
  double voc = open_circuit_voltage;
  double r   = internal_resistance;

  double term1 = 1.0 / (2.0 * r);

  double term2_first_den = gsl_pow_2(voc / (2.0 * r)) - p / r;
  double term2_first     = 1 / sqrt(term2_first_den);
  double term2_second    = voc / (4 * gsl_pow_2(r));
  double term2           = term2_first * term2_second;
  return term1 - term2;
}

double lion_current_optimize_targetfn(double current, void *params) {
  struct lion_optimization_iter_params *p            = params;
  double                                rint         = lion_resistance(p->soc, current, p->params);
  double                                pred_current = lion_current(p->power, p->voc, rint, p->params);
  double                                val          = gsl_pow_2(fabs(current - pred_current));
  return val;
}

double lion_current_optimize(
    gsl_min_fminimizer *s,
    double              power,
    double              soc,
    double              open_circuit_voltage,
    double              initial_guess,
    double              epsabs,
    double              epsrel,
    int                 max_iter,
    lion_params_t      *params
) {
  // The goal is to find the current I that solves the equation I = f(I)
  // where f is some known equation. The issue is that f might no be invertible
  // or it might have a non-analitic inverse, thus I is found by solving the
  // optimization problem
  //   min ||I - f(I)||^2
  struct lion_optimization_iter_params opt_params = {
      .power  = power,
      .voc    = open_circuit_voltage,
      .soc    = soc,
      .params = params,
  };

  double opt_min = LION_CURRENT_OPTMIN;
  double opt_max = LION_CURRENT_OPTMAX;

  gsl_function F;

  F.function = &lion_current_optimize_targetfn;
  F.params   = &opt_params;
  gsl_min_fminimizer_set(s, &F, initial_guess, opt_min, opt_max);

  int status;
  int iter = 0;
  do {
    iter++;
    status = gsl_min_fminimizer_iterate(s);

    initial_guess = gsl_min_fminimizer_x_minimum(s);
    opt_min       = gsl_min_fminimizer_x_lower(s);
    opt_max       = gsl_min_fminimizer_x_upper(s);

    status = gsl_min_test_interval(opt_min, opt_max, epsabs, epsrel);
  } while (status == GSL_CONTINUE && iter < max_iter);
  if (status != GSL_SUCCESS) {
    logi_error("Current did not converge");
  }
  return initial_guess;
}
