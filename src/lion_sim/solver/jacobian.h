#pragma once

#include <lion/sim.h>

double jac_0_0_analytical(lion_sim_state_t *state, lion_params_t *params);
double jac_0_1_analytical(lion_sim_state_t *state, lion_params_t *params);
double jac_1_0_analytical(lion_sim_state_t *state, lion_params_t *params);
double jac_1_1_analytical(lion_sim_state_t *state, lion_params_t *params);
double jac_0_t_analytical(lion_sim_state_t *state, lion_params_t *params);
double jac_1_t_analytical(lion_sim_state_t *state, lion_params_t *params);

double jac_0_0_2point(lion_sim_state_t *state, lion_params_t *params);
double jac_0_1_2point(lion_sim_state_t *state, lion_params_t *params);
double jac_1_0_2point(lion_sim_state_t *state, lion_params_t *params);
double jac_1_1_2point(lion_sim_state_t *state, lion_params_t *params);
double jac_0_t_2point(lion_sim_state_t *state, lion_params_t *params);
double jac_1_t_2point(lion_sim_state_t *state, lion_params_t *params);
