#include <gsl/gsl_odeiv2.h>

#include <lion/lion.h>
#include <lion_utils/macros.h>
#include <lion_utils/vendor/log.h>

#include "app_run.h"

lion_status_t lion_app_init(lion_app_t *app, double initial_power,
                            double initial_amb_temp) {
  // Simulation stepper configuration
  logi_debug("Configuring simulation stepper");
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

  // Optimization problem minimizer
  logi_debug("Configuring optimization minimizer");
  switch (app->conf->sim_minimizer) {
  case LION_MINIMIZER_GOLDENSECTION:
    app->minimizer = gsl_min_fminimizer_goldensection;
  case LION_MINIMIZER_BRENT:
    app->minimizer = gsl_min_fminimizer_brent;
  case LION_MINIMIZER_QUADGOLDEN:
    app->minimizer = gsl_min_fminimizer_quad_golden;
  default:
    logi_error("Desired minimizer not implemented");
    return LION_STATUS_FAILURE;
  }

  // Initial conditions
  logi_info("Configuring initial state");
  logi_debug("Setting up initial conditions");
  app->state.soc_nominal = app->params->init.initial_soc;
  app->state.internal_temperature =
      app->params->init.initial_internal_temperature;
  app->state.time = 0.0;
  logi_debug("Setting first inputs");
  app->state.power = initial_power;
  app->state.ambient_temperature = initial_amb_temp;
  LION_CALL_I(lion_slv_update(app), "Failed spreading initial condition");

  // System configuration
  logi_info("Configuring ode system");
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

  // Set up driver
  logi_info("Configuring simulation driver");
  app->driver = gsl_odeiv2_driver_alloc_y_new(
      &app->sys, app->step_type, app->conf->sim_step_seconds,
      app->conf->sim_epsabs, app->conf->sim_epsrel);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_simulate(lion_app_t *app, lion_vector_t *power,
                                lion_vector_t *amb_temp) {
  uint64_t max_iters = lion_app_max_iters(app);
  logi_debug("Considering %d max iterations", max_iters);

  if (app->init_hook != NULL) {
    logi_debug("Found init hook");
    LION_CALLDF_I(app->init_hook(app), "Failed calling init hook");
  }

  logi_debug("Starting iterations");
  for (uint64_t i = 0; i < max_iters; i++) {
    // app->state contains state(k)
    double partial_result[2];
    LION_GSL_CALL_I(gsl_odeiv2_driver_apply_fixed_step(
                        app->driver, &app->state.time,
                        app->conf->sim_step_seconds, 1, partial_result),
                    "Failed at step %d (t = %f)", i, app->state.time);
    // state(k + 1) is stored in partial_result
    app->state.soc_nominal = partial_result[0];
    app->state.internal_temperature = partial_result[1];
    app->state.power = lion_vector_get_d(app, power, i);
    app->state.ambient_temperature = lion_vector_get_d(app, amb_temp, i);
    LION_CALL_I(lion_slv_update(app), "Failed updating state");
    // partial results are spread over the state, meaning at this
    // point app->state contains state(k + 1) leaving it ready for the
    // next time iteration

    if (app->update_hook != NULL) {
      // TODO: Evaluate implementation of concurrency
      // TODO: Add some mechanism to avoid race conditions
      LION_CALLDF_I(app->update_hook(app, i), "Failed calling update hook");
    }
  }

  logi_debug("Finished iterations");
  if (app->finished_hook != NULL) {
    logi_debug("Found finished hook");
    LION_CALLDF_I(app->finished_hook(app), "Failed calling finished hook");
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
