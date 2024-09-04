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

  // System configuration
  logi_debug("Configuring initial state");
  app->state->soc_nominal = app->params->init.initial_soc;
  app->state->internal_temperature =
      app->params->init.initial_internal_temperature;
  app->state->time = 0.0;
  LION_CALL_I(lion_slv_update(app, initial_power, initial_amb_temp),
              "Failed spreading initial condition");
  // TODO: Implement setting up ODE system

  // Set up driver
  logi_debug("Configuring simulation driver");
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
    double partial_result[2];
    LION_GSL_CALL_I(gsl_odeiv2_driver_apply_fixed_step(
                        app->driver, &app->state->time,
                        app->conf->sim_step_seconds, 1, partial_result),
                    "Failed at step %d (t = %f)", i, app->state->time);
    app->state->soc_nominal = partial_result[0];
    app->state->internal_temperature = partial_result[1];
    LION_CALL_I(lion_slv_update(app, lion_vector_get_d(app, power, i),
                                lion_vector_get_d(app, amb_temp, i)),
                "Failed updating state");

    if (app->update_hook != NULL) {
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
