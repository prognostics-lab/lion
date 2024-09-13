#include <gsl/gsl_odeiv2.h>

#include <lion/lion.h>
#include <lion_utils/macros.h>
#include <lion_utils/vendor/log.h>

#include "app_run.h"

lion_status_t _init_simulation_stepper(lion_app_t *app) {
  switch (app->conf->sim_stepper) {
  case LION_STEPPER_RK2:
    app->step_type = gsl_odeiv2_step_rk2;
    break;
  case LION_STEPPER_RK4:
    app->step_type = gsl_odeiv2_step_rk4;
    break;
  case LION_STEPPER_RKF45:
    app->step_type = gsl_odeiv2_step_rkf45;
    break;
  case LION_STEPPER_RKCK:
    app->step_type = gsl_odeiv2_step_rkck;
    break;
  case LION_STEPPER_RK8PD:
    app->step_type = gsl_odeiv2_step_rk8pd;
    break;
  case LION_STEPPER_RK1IMP:
    app->step_type = gsl_odeiv2_step_rk1imp;
    break;
  case LION_STEPPER_RK2IMP:
    app->step_type = gsl_odeiv2_step_rk2imp;
    break;
  case LION_STEPPER_RK4IMP:
    app->step_type = gsl_odeiv2_step_rk4imp;
    break;
  case LION_STEPPER_BSIMP:
    app->step_type = gsl_odeiv2_step_bsimp;
    break;
  case LION_STEPPER_MSADAMS:
    app->step_type = gsl_odeiv2_step_msadams;
    break;
  case LION_STEPPER_MSBDF:
    app->step_type = gsl_odeiv2_step_msbdf;
    break;
  default:
    logi_error("Desired step type not implemented");
    return LION_STATUS_FAILURE;
  }
  return LION_STATUS_SUCCESS;
}

lion_status_t _init_simulation_minimizer(lion_app_t *app) {
  switch (app->conf->sim_minimizer) {
  case LION_MINIMIZER_GOLDENSECTION:
    app->minimizer = gsl_min_fminimizer_goldensection;
    break;
  case LION_MINIMIZER_BRENT:
    app->minimizer = gsl_min_fminimizer_brent;
    break;
  case LION_MINIMIZER_QUADGOLDEN:
    app->minimizer = gsl_min_fminimizer_quad_golden;
    break;
  default:
    logi_error("Desired minimizer not implemented");
    return LION_STATUS_FAILURE;
  }
  app->sys_min = gsl_min_fminimizer_alloc(app->minimizer);
  logi_info("Using minimizer %s", gsl_min_fminimizer_name(app->sys_min));
  return LION_STATUS_SUCCESS;
}

lion_status_t _init_initial_state(lion_app_t *app, double initial_power,
                                  double initial_amb_temp) {
  logi_debug("Setting up initial conditions");
  // The current is set at first because it is used as an initial guess
  // for the optimization problem
  app->state.current = 0.0;
  app->state.soc_nominal = app->params->init.initial_soc;
  app->state.internal_temperature =
      app->params->init.initial_internal_temperature;
  app->state.time = 0.0;
  app->state.step = 0;
  logi_debug("Setting first inputs");
  app->state.power = initial_power;
  app->state.ambient_temperature = initial_amb_temp;
  LION_CALL_I(lion_slv_update(app), "Failed spreading initial condition");
  return LION_STATUS_SUCCESS;
}

lion_status_t _init_ode_system(lion_app_t *app) {
  logi_debug("Setting up GSL inputs");
  app->inputs.sys_inputs = &app->state;
  app->inputs.sys_params = app->params;
  logi_debug("Creating GSL system");
  gsl_odeiv2_system sys = {
      .function = &lion_slv_system,
      .jacobian = &lion_slv_jac,
      .dimension = LION_SLV_DIMENSION,
      .params = &app->inputs,
  };
  app->sys = sys;
  return LION_STATUS_SUCCESS;
}

lion_status_t _init_ode_driver(lion_app_t *app) {
  app->driver = gsl_odeiv2_driver_alloc_y_new(
      &app->sys, app->step_type, app->conf->sim_step_seconds,
      app->conf->sim_epsabs, app->conf->sim_epsrel);
  return LION_STATUS_SUCCESS;
}

lion_status_t _show_init_information(lion_app_t *app) {
  logi_debug("Initial state");
  logi_debug("|-> P                : %f W", app->state.power);
  logi_debug("|-> T_amb            : %f °C", app->state.ambient_temperature);
  logi_debug("|-> V_terminal       : %f V", app->state.voltage);
  logi_debug("|-> I                : %f A", app->state.current);
  logi_debug("|-> V_oc             : %f V", app->state.open_circuit_voltage);
  logi_debug("|-> R_int            : %f Ohm", app->state.internal_resistance);
  logi_debug("|-> EHC              : %f V/K", app->state.ehc);
  logi_debug("|-> q_gen            : %f W", app->state.generated_heat);
  logi_debug("|-> T_in             : %f °C", app->state.internal_temperature);
  logi_debug("|-> T_s              : %f °C", app->state.surface_temperature);
  logi_debug("|-> kappa            : %f", app->state.kappa);
  logi_debug("|-> SoC_0            : %f", app->state.soc_nominal);
  logi_debug("|-> SoC_use          : %f", app->state.soc_use);
  logi_debug("|-> Q_0              : %f C", app->state.capacity_nominal);
  logi_debug("|-> Q_use            : %f C", app->state.capacity_use);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_init(lion_app_t *app, double initial_power,
                            double initial_amb_temp) {
  logi_debug("Configuring simulation stepper");
  LION_CALL_I(_init_simulation_stepper(app),
              "Failed initializing simulation stepper");

  logi_debug("Configuring optimization minimizer");
  LION_CALL_I(_init_simulation_minimizer(app),
              "Failed initializing simulation minimizer");

  logi_info("Configuring initial state");
  LION_CALL_I(_init_initial_state(app, initial_power, initial_amb_temp),
              "Failed initializing initial state");

  logi_info("Configuring ode system");
  LION_CALL_I(_init_ode_system(app), "Failed initializing ode system");

  logi_info("Configuring simulation driver");
  LION_CALL_I(_init_ode_driver(app), "Failed initializing ode driver");

  LION_CALL_I(_show_init_information(app),
              "Failed showing initialization information");
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_simulate(lion_app_t *app, lion_vector_t *power,
                                lion_vector_t *amb_temp) {
  // uint64_t max_iters = lion_app_max_iters(app);
  uint64_t max_iters = power->len;
  logi_debug("Considering %d max iterations", max_iters);

  if (app->conf->init_hook != NULL) {
    logi_debug("Found init hook");
    LION_CALLDF_I(app->conf->init_hook(app), "Failed calling init hook");
  }

  logi_debug("Starting iterations");
  for (uint64_t i = 1; i < max_iters; i++) {
    logi_debug("Step %d", i);
    if (i == power->len || i == amb_temp->len) {
      logi_error("Ran out of inputs before reaching end of simulation");
      break;
    }
    LION_VCALL_I(lion_app_step(app, lion_vector_get_d(app, power, i),
                               lion_vector_get_d(app, amb_temp, i)),
                 "Failed at iteration %i", i);
  }

  logi_debug("Finished iterations");
  if (app->conf->finished_hook != NULL) {
    logi_debug("Found finished hook");
    LION_CALLDF_I(app->conf->finished_hook(app),
                  "Failed calling finished hook");
  }
  return LION_STATUS_SUCCESS;
}

#ifndef NDEBUG
lion_status_t lion_app_init_debug(lion_app_t *app) {
  app->_idebug_malloced_total = 0;
  _idebug_heap_info_t *head = heapinfo_new(app);
  if (head == NULL) {
    logi_error("Could not allocate memory for head node of heap info");
    return LION_STATUS_FAILURE;
  }
  app->_idebug_heap_head = head;

  return LION_STATUS_SUCCESS;
}
#endif
