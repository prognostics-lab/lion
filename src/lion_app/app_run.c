#include <gsl/gsl_odeiv2.h>
#include <math.h>

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

#define _SHOW_STATE(app, f)                                                    \
  f("Cell state");                                                             \
  f("|-> P                : %f W", app->state.power);                          \
  f("|-> T_amb            : %f K", app->state.ambient_temperature);            \
  f("|-> V_terminal       : %f V", app->state.voltage);                        \
  f("|-> I                : %f A", app->state.current);                        \
  f("|-> V_oc             : %f V", app->state.open_circuit_voltage);           \
  f("|-> R_int            : %f Ohm", app->state.internal_resistance);          \
  f("|-> EHC              : %f V/K", app->state.ehc);                          \
  f("|-> q_gen            : %f W", app->state.generated_heat);                 \
  f("|-> T_in             : %f K", app->state.internal_temperature);           \
  f("|-> T_s              : %f K", app->state.surface_temperature);            \
  f("|-> kappa            : %f", app->state.kappa);                            \
  f("|-> SoC_0            : %f", app->state.soc_nominal);                      \
  f("|-> SoC_use          : %f", app->state.soc_use);                          \
  f("|-> Q_0              : %f C", app->state.capacity_nominal);               \
  f("|-> Q_use            : %f C", app->state.capacity_use);

lion_status_t lion_app_show_state_info(lion_app_t *app) {
  _SHOW_STATE(app, logi_info);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_show_state_debug(lion_app_t *app) {
  _SHOW_STATE(app, logi_debug);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_show_state_trace(lion_app_t *app) {
  _SHOW_STATE(app, logi_trace);
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

  LION_CALL_I(lion_app_show_state_debug(app),
              "Failed showing initialization information");
  return LION_STATUS_SUCCESS;
}

void _template_progressbar(FILE *buf, int width) {
  int x = 0;
  for (x = 0; x < width; x++) {
    fprintf(buf, " ");
  }
  fprintf(buf, "]");
}

void _update_progressbar(FILE *buf, int i, int max, int width, int *c,
                         int *last_c) {
  double progress = i * 100.0 / max;
  *c = (int)progress;
  fprintf(buf, "\n\033[F");
  fprintf(buf, "%3d%%", *c);
  fprintf(buf, "\033[1C");
  fprintf(buf, "\033[%dC=", *last_c);
  int x = 0;
  for (x = *last_c; x < *c; x++) {
    fprintf(buf, "=");
  }
  if (x < width) {
    fprintf(buf, ">");
  }
  *last_c = *c;
}

void _finish_progressbar(FILE *buf) { fprintf(stderr, "\033[EDone\n"); }

lion_status_t lion_app_simulate(lion_app_t *app, lion_vector_t *power,
                                lion_vector_t *amb_temp) {

  uint64_t max_iters = fminl(max_iters, power->len);
  max_iters = fminl(max_iters, amb_temp->len);
  logi_debug("Considering %d max iterations", max_iters);

  // Initialization hook
  if (app->conf->init_hook != NULL) {
    logi_debug("Found init hook");
    LION_CALLDF_I(app->conf->init_hook(app), "Failed calling init hook");
  }

  logi_debug("Starting iterations");
  _template_progressbar(stderr, LION_PROGRESSBAR_WIDTH);
  int c = 0;
  int last_c = 0;
  for (uint64_t i = 1; i < max_iters; i++) {
    _update_progressbar(stderr, i, max_iters, LION_PROGRESSBAR_WIDTH, &c,
                        &last_c);

    logi_trace("Step %d", i);
    if (i == power->len || i == amb_temp->len) {
      logi_error("Ran out of inputs before reaching end of simulation");
      break;
    }
    LION_VCALL_I(lion_app_step(app, lion_vector_get_d(app, power, i),
                               lion_vector_get_d(app, amb_temp, i)),
                 "Failed at iteration %i", i);
  }
  _finish_progressbar(stderr);

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
