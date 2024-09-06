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

double jac_0_0(lion_app_state_t *state, lion_params_t *params) {
  double term1 =
      lion_current_grad_voc(state->power, state->open_circuit_voltage,
                            state->internal_resistance, params);
  double term2 = lion_voc_grad(state->soc_use, params);
  return -term1 * term2 * state->kappa / state->capacity_use;
}

double jac_0_1(lion_app_state_t *state, lion_params_t *params) {
  double numl_term1 =
      lion_current_grad_voc(state->power, state->open_circuit_voltage,
                            state->internal_resistance, params);
  double numl_term2 = lion_voc_grad(state->soc_use, params);
  double numl_term3 = state->soc_nominal;
  double numl_term4 = lion_kappa_grad(state->internal_temperature, params);
  double numl_coeff = numl_term1 * numl_term2 * numl_term3 * numl_term4;
  double numl = state->capacity_use * numl_coeff;

  double numr_term1 = numl_term4;
  double numr = state->current * state->capacity_nominal * numr_term1;

  double den = gsl_pow_2(state->capacity_use);
  return (numl - numr) / den;
}

double jac_1_0(lion_app_state_t *state, lion_params_t *params) {
  double term1_1 = 2.0 * state->internal_resistance * state->current;
  double term1_2 = state->internal_temperature * state->ehc;
  double term1 = term1_1 - term1_2;
  double term2 =
      lion_current_grad_voc(state->power, state->open_circuit_voltage,
                            state->internal_resistance, params);
  double term3 = lion_voc_grad(state->soc_use, params);
  return term1 * term2 * term3 * state->kappa / params->t.cp;
}

double jac_1_1(lion_app_state_t *state, lion_params_t *params) {
  double rt = params->t.rin + params->t.rout;
  double t = 1.0 / (params->t.cp * rt);
  return -t - state->ehc / params->t.cp;
}

int lion_slv_jac(double t, const double state[], double *dfdy, double dfdt[],
                 void *inputs) {
  lion_slv_inputs_t *p = inputs;
  lion_app_state_t *sys_state = p->sys_inputs;
  lion_params_t *sys_params = p->sys_params;

  (void)t;
  gsl_matrix_view dfdy_mat = gsl_matrix_view_array(dfdy, 2, 2);
  gsl_matrix *m = &dfdy_mat.matrix;
  gsl_matrix_set(m, 0, 0, jac_0_0(sys_state, sys_params));
  gsl_matrix_set(m, 0, 1, jac_0_1(sys_state, sys_params));
  gsl_matrix_set(m, 1, 0, jac_1_0(sys_state, sys_params));
  gsl_matrix_set(m, 1, 1, jac_1_1(sys_state, sys_params));
  dfdt[0] = 0.0;
  dfdt[1] = 0.0;
  return GSL_SUCCESS;
}
