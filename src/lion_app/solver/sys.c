#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>

#include <lion/app.h>
#include <lion/params.h>
#include <lion/solver/inputs.h>
#include <lion/solver/sys.h>
#include <lion_math/capacity.h>
#include <lion_math/current.h>
#include <lion_math/dynamics/soc.h>
#include <lion_math/dynamics/temperature.h>
#include <lion_math/internal_resistance.h>
#include <lion_math/open_circuit.h>

int lion_slv_system(double t, const double state[], double out[],
                    void *inputs) {
  /*
     state[0] -> state of charge
     state[1] -> internal temperature

     inputs[0] -> *lion_app_state_t
     inputs[1] -> *lion_params_t
   */

  // void **p = (void **)inputs;
  // void *sys_inputs_p = p[0];
  // void *sys_params_p = p[1];
  // double *sys_inputs = (double *)sys_inputs_p;
  // lion_params_t *sys_params = (lion_params_t *)sys_params_p;
  lion_slv_inputs_t *p = inputs;
  lion_app_state_t *sys_inputs = p->sys_inputs;
  lion_params_t *sys_params = p->sys_params;

  (void)t;
  out[0] =
      lion_soc_d(sys_inputs->current, sys_inputs->capacity_use, sys_params);
  out[1] =
      lion_internal_temperature_d(state[1], sys_inputs->internal_temperature,
                                  sys_inputs->ambient_temperature, sys_params);
  return GSL_SUCCESS;
}

double jac_0_1(double soc_use, double kappa, double r, double voc, double p,
               double q_use, lion_params_t *params) {
  double term1 = lion_current_grad_voc(p, voc, r, params);
  double term2 = lion_voc_grad(soc_use, params);
  return -term1 * term2 * kappa / q_use;
}

int lion_slv_jac(double t, const double state[], double *dfdy, double dfdt[],
                 void *inputs) {
  lion_slv_inputs_t *p = inputs;
  lion_app_state_t *sys_inputs = p->sys_inputs;
  lion_params_t *sys_params = p->sys_params;

  double kappa = lion_kappa(state[1], sys_params);
  double soc_use = lion_soc_usable(state[0], kappa, sys_params);
  double rint = lion_resistance(sys_inputs->current, sys_params);

  (void)t;
  gsl_matrix_view dfdy_mat = gsl_matrix_view_array(dfdy, 2, 2);
  gsl_matrix *m = &dfdy_mat.matrix;
  // TODO: Finish Jacobian calculation
  gsl_matrix_set(m, 0, 0, 0.0);
  gsl_matrix_set(m, 0, 1, 1.0);
  dfdt[0] = 0.0;
  dfdt[1] = 0.0;
  return GSL_SUCCESS;

  return GSL_SUCCESS;
}
