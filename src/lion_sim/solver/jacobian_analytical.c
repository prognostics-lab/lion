#include "jacobian.h"

#include <lion/sim.h>
#include <lion_math/capacity.h>
#include <lion_math/current.h>
#include <lion_math/open_circuit.h>

double jac_0_0_analytical(lion_sim_state_t *state, lion_params_t *params) {
  double term1 = lion_current_grad_voc(state->power, state->open_circuit_voltage, state->internal_resistance, params);
  double term2 = lion_voc_grad(state->soc_use, params);
  return -term1 * term2 * state->kappa / state->capacity_use;
}

double jac_0_1_analytical(lion_sim_state_t *state, lion_params_t *params) {
  double numl_term1 = lion_current_grad_voc(state->power, state->open_circuit_voltage, state->internal_resistance, params);
  double numl_term2 = lion_voc_grad(state->soc_use, params);
  double numl_term3 = state->soc_nominal;
  double numl_term4 = lion_kappa_grad(state->internal_temperature, params);
  double numl_coeff = numl_term1 * numl_term2 * numl_term3 * numl_term4;
  double numl       = state->capacity_use * numl_coeff;

  double numr_term1 = numl_term4;
  double numr       = state->current * state->capacity_nominal * numr_term1;

  double den = gsl_pow_2(state->capacity_use);
  return (numl - numr) / den;
}

double jac_1_0_analytical(lion_sim_state_t *state, lion_params_t *params) {
  double term1_1 = 2.0 * state->internal_resistance * state->current;
  double term1_2 = state->internal_temperature * state->ehc;
  double term1   = term1_1 - term1_2;
  double term2   = lion_current_grad_voc(state->power, state->open_circuit_voltage, state->internal_resistance, params);
  double term3   = lion_voc_grad(state->soc_use, params);
  return term1 * term2 * term3 * state->kappa / params->temp.cp;
}

double jac_1_1_analytical(lion_sim_state_t *state, lion_params_t *params) {
  double rt = params->temp.rin + params->temp.rout;
  double t  = 1.0 / (params->temp.cp * rt);
  return -t - state->ehc / params->temp.cp;
}

double jac_0_t_analytical(lion_sim_state_t *state, lion_params_t *params) { return 0.0; }

double jac_1_t_analytical(lion_sim_state_t *state, lion_params_t *params) { return 0.0; }
