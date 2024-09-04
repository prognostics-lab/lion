#include <gsl/gsl_odeiv2.h>

#include <lion/lion.h>
#include <lion_utils/vendor/log.h>

#include "app_run.h"

lion_status_t lion_app_init(lion_app_t *app) {
  // Simulation stepper configuration
  logi_debug("Configuring simulation stepper");
  switch (app->conf->sim_stepper) {
  case TERRA_STEPPER_RK2:
    app->step_type = gsl_odeiv2_step_rk2;
    break;
  case TERRA_STEPPER_RK4:
    app->step_type = gsl_odeiv2_step_rk4;
    break;
  case TERRA_STEPPER_RKF45:
    app->step_type = gsl_odeiv2_step_rkf45;
    break;
  case TERRA_STEPPER_RKCK:
    app->step_type = gsl_odeiv2_step_rkck;
    break;
  case TERRA_STEPPER_RK8PD:
    app->step_type = gsl_odeiv2_step_rk8pd;
    break;
  case TERRA_STEPPER_RK1IMP:
    app->step_type = gsl_odeiv2_step_rk1imp;
    break;
  case TERRA_STEPPER_RK2IMP:
    app->step_type = gsl_odeiv2_step_rk2imp;
    break;
  case TERRA_STEPPER_RK4IMP:
    app->step_type = gsl_odeiv2_step_rk4imp;
    break;
  case TERRA_STEPPER_BSIMP:
    app->step_type = gsl_odeiv2_step_bsimp;
    break;
  case TERRA_STEPPER_MSADAMS:
    app->step_type = gsl_odeiv2_step_msadams;
    break;
  case TERRA_STEPPER_MSBDF:
    app->step_type = gsl_odeiv2_step_msbdf;
    break;
  default:
    logi_error("Desired step type not implemented");
    return LION_STATUS_FAILURE;
  }

  // System configuration
  logi_debug("Configuring ode system");

  // Set up driver
  logi_debug("Configuring simulation driver");
  app->driver = gsl_odeiv2_driver_alloc_y_new(
      &app->sys, app->step_type, app->conf->sim_step_seconds,
      app->conf->sim_epsabs, app->conf->sim_epsrel);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_simulate(lion_app_t *app) {
  uint64_t max_iters =
      (uint64_t)(app->conf->sim_time_seconds / app->conf->sim_step_seconds);
  logi_debug("Considering %d max iterations", max_iters);
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
