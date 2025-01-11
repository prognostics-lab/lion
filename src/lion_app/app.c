#include "app_run.h"
#include "mem.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv2.h>
#include <lion/lion.h>
#include <lion_utils/macros.h>
#include <lion_utils/vendor/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>

#ifdef _WIN32
  #include <windows.h>
  #define CREATE_DIRECTORY(dirname)     CreateDirectory(dirname, NULL)
  #define DIR_ALREADY_EXISTS_ERROR_CODE ERROR_ALREADY_EXISTS
  #define EPOCH_DIFF                    11644473600000ULL // Milliseconds
  #define GET_CURRENT_TIME(current_time)                                                                                                             \
    {                                                                                                                                                \
      FILETIME       ft;                                                                                                                             \
      ULARGE_INTEGER ft64;                                                                                                                           \
      GetSystemTimeAsFileTime(&ft);                                                                                                                  \
      ft64.LowPart  = ft.dwLowDateTime;                                                                                                              \
      ft64.HighPart = ft.dwHighDateTime;                                                                                                             \
      current_time  = (int64_t)(ft64.QuadPart / 10000 - EPOCH_DIFF);                                                                                 \
    }
#else
  #include <errno.h>
  #include <sys/stat.h>
  #include <sys/time.h>
  #define CREATE_DIRECTORY(dirname)     mkdir(dirname, 0777)
  #define DIR_ALREADY_EXISTS_ERROR_CODE EEXIST
  #define GET_CURRENT_TIME(current_time)                                                                                                             \
    {                                                                                                                                                \
      struct timeval tv;                                                                                                                             \
      gettimeofday(&tv, NULL);                                                                                                                       \
      current_time = (int64_t)tv.tv_sec * 1000LL + (int64_t)tv.tv_usec / 1000LL;                                                                     \
    }
#endif

const lion_app_config_t LION_APP_CONFIG_DEFAULT = {
  // Metadata
  .app_name = "Application",

  // Simulation parameters
  .sim_stepper      = LION_STEPPER_RKF45,
  .sim_minimizer    = LION_MINIMIZER_BRENT,
  .sim_jacobian     = LION_JACOBIAN_ANALYTICAL,
  .sim_time_seconds = 10.0,
  .sim_step_seconds = 1e-3,
  .sim_epsabs       = 1e-8,
  .sim_epsrel       = 1e-8,

  // Logging
  .log_dir     = NULL,
  .log_stdlvl  = LOG_INFO,
  .log_filelvl = LOG_TRACE,
};

lion_status_t lion_app_config_new(lion_app_config_t *out) {
  lion_app_config_t conf = LION_APP_CONFIG_DEFAULT;
  *out                   = conf;
  return LION_STATUS_SUCCESS;
}

lion_app_config_t lion_app_config_default(void) { return LION_APP_CONFIG_DEFAULT; }

static lion_status_t create_directory(const char *dirname) {
#ifdef _WIN32
  if (CREATE_DIRECTORY(dirname) == 0) {
    DWORD error = GetLastError();
    if (error == DIR_ALREADY_EXISTS_ERROR_CODE) {
      logi_debug("Log directory '%s' already exists", dirname);
      return LION_STATUS_SUCCESS;
    } else {
      logi_error("Failed to create directory '%s'", dirname);
      return LION_STATUS_FAILURE;
    }
  }
#else
  if (CREATE_DIRECTORY(dirname) == -1) {
    if (errno == DIR_ALREADY_EXISTS_ERROR_CODE) {
      logi_debug("Log directory '%s' already exists", dirname);
      return LION_STATUS_SUCCESS;
    } else {
      logi_error("Failed to create directory '%s'", dirname);
      return LION_STATUS_FAILURE;
    }
  }
#endif
  logi_debug("Succesfully created directory '%s'", dirname);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_new(lion_app_config_t *conf, lion_params_t *params, lion_app_t *out) {
  lion_app_t app = {
    .conf          = conf,
    .params        = params,
    .init_hook     = NULL,
    .update_hook   = NULL,
    .finished_hook = NULL,

    .driver    = NULL,
    .sys_min   = NULL,
    .step_type = NULL,
    .minimizer = NULL,

#ifndef NDEBUG // Internal debug information
    ._idebug_malloced_total = 0,
#endif
  };

  // Logging setup
  time_t     seconds = time(NULL);
  struct tm *time    = localtime(&seconds);
  log_set_level(app.conf->log_stdlvl);

  if (app.conf->log_dir == NULL) {
    logi_warn("Log directory not specified, not logging to file");
  } else {
    if (create_directory(app.conf->log_dir) == LION_STATUS_SUCCESS) {
      size_t log_dir_len = strnlen(app.conf->log_dir, FILENAME_MAX);
      strncpy(app.log_filename, app.conf->log_dir, log_dir_len);

      app.log_filename[strftime(app.log_filename + log_dir_len, _LION_LOGFILE_MAX, "/%Y%m%d_%H%M%S.txt", time) + log_dir_len] = '\0';

      app.log_file = fopen(app.log_filename, "w");
      if (app.log_file == NULL) {
        logi_error("Failed to create log file, not logging to file");
      } else {
        logi_info("Log file : '%s'", app.log_filename);
        log_add_fp_internal(app.log_file, app.conf->log_filelvl);
      }
    } else {
      logi_error("Failed to create log directory, not logging to file");
    }
  }

#ifndef NDEBUG
  LION_CALL_I(lion_app_init_debug(&app), "Failed initializing debug information");
#endif
  *out = app;
  return LION_STATUS_SUCCESS;
}

static void lion_app_log_startup_info(lion_app_t *app) {
#ifndef NDEBUG
  logi_warn("!!! RUNNING IN DEBUG MODE !!!");
#endif
  logi_info("+-------------------------------------------------------+");
  logi_info("|################# STARTUP INFORMATION #################|");
  logi_info("+-------------------------------------------------------+");
  logi_info(" * Application name               : %s", app->conf->app_name);
#ifdef LION_ENGINE_VERSION_MAJOR
  lion_version_t ver = lion_app_get_version(app);
  logi_info(" * Engine version                 : %s.%s.%s", ver.major, ver.minor, ver.patch);
#else
  logi_info(" * Engine version                 : N/A");
#endif
  logi_info(" * `lion_app_t` struct size       : %d B", sizeof(lion_app_t));
  if (app->conf->log_dir != NULL) {
    logi_info(" * Log directory                  : %s", app->conf->log_dir);
    logi_info(" * Log std level                  : %d", app->conf->log_stdlvl);
    logi_info(" * Log file level                 : %d", app->conf->log_filelvl);
  }
  logi_info(" * Regime                         : %s", lion_app_regime_name(app->conf->sim_regime));
  logi_info(" * Stepper                        : %s", lion_app_stepper_name(app->conf->sim_stepper));
  logi_info(" * Minimizer                      : %s", lion_app_minimizer_name(app->conf->sim_minimizer));
  logi_info(" * Jacobian                       : %s", lion_jacobian_name(app->conf->sim_jacobian));
  logi_info(" * Total simulation time          : %f s", app->conf->sim_time_seconds);
  logi_info(" * Simulation step time           : %f s", app->conf->sim_step_seconds);
  logi_info(" * Absolute epsilon               : %f", app->conf->sim_epsabs);
  logi_info(" * Relative epsilon               : %f", app->conf->sim_epsrel);
  logi_info(" * Minimization max iterations    : %d iterations", app->conf->sim_min_maxiter);
  if (app->init_hook != NULL) {
    logi_info(" * Init hook                      : YES");
  } else {
    logi_info(" * Init hook                      : NO");
  }
  if (app->update_hook != NULL) {
    logi_info(" * Update hook                    : YES");
  } else {
    logi_info(" * Update hook                    : NO");
  }
  if (app->finished_hook != NULL) {
    logi_info(" * Finished hook                  : YES");
  } else {
    logi_info(" * Finished hook                  : NO");
  }
  logi_info(" * Initialization parameters");
  logi_info(" |-> State of charge              : %f %%", 100.0 * app->params->init.soc);
  logi_info(" |-> State of health              : %f %%", 100.0 * app->params->init.soh);
  logi_info(" |-> Internal temperature         : %f K", app->params->init.temp_in);
  logi_info(" |-> Nominal capacity             : %f C (%f Ah)", app->params->init.capacity, app->params->init.capacity / 3600.0);
  logi_info(" |-> Current guess                : %f A", app->params->init.current_guess);
  logi_info(" * Entropic heat coefficient parameters");
  logi_info(" |-> a                            : %f V/K", app->params->ehc.a);
  logi_info(" |-> b                            : %f V/K", app->params->ehc.b);
  logi_info(" |-> kappa                        : %f", app->params->ehc.kappa);
  logi_info(" |-> mu                           : %f", app->params->ehc.mu);
  logi_info(" |-> sigma                        : %f", app->params->ehc.sigma);
  logi_info(" |-> l                            : %f", app->params->ehc.l);
  logi_info(" * Open circuit voltage");
  logi_info(" |-> alpha                        : %f", app->params->ocv.alpha);
  logi_info(" |-> beta                         : %f", app->params->ocv.beta);
  logi_info(" |-> gamma                        : %f", app->params->ocv.gamma);
  logi_info(" |-> v0                           : %f V", app->params->ocv.v0);
  logi_info(" |-> vl                           : %f V", app->params->ocv.vl);
  logi_info(" * Vogel-Fulcher-Tammann");
  logi_info(" |-> k1                           : %f K", app->params->vft.k1);
  logi_info(" |-> k2                           : %f K", app->params->vft.k2);
  logi_info(" |-> Reference temperature        : %f K", app->params->vft.tref);
  logi_info(" * Temperature model");
  logi_info(" |-> Heat capacity                : %f J K-1", app->params->temp.cp);
  logi_info(" |-> Internal thermal resistivity : %f K W-1", app->params->temp.rin);
  logi_info(" |-> Outer thermal resistivity   : %f K W-1", app->params->temp.rout);
  logi_info("+-------------------------------------------------------+");
  logi_info("|################# END OF INFORMATION ##################|");
  logi_info("+-------------------------------------------------------+");
}

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

lion_status_t _init_initial_state(lion_app_t *app) {
  logi_debug("Setting up initial conditions");
  // The current is set at first because it is used as an initial guess
  // for the optimization problem
  app->state.current                    = 0.0;
  app->state._next_soc_nominal          = app->params->init.soc;
  app->state._next_internal_temperature = app->params->init.temp_in;
  app->state.time                       = 0.0;
  app->state.step                       = 0;
  // logi_debug("Setting first inputs, should be irrelevant");
  // app->state.power               = 0.0;
  // app->state.ambient_temperature = app->params->init.temp_in;
  return LION_STATUS_SUCCESS;
}

lion_status_t _init_ode_system(lion_app_t *app) {
  logi_debug("Setting up GSL inputs");
  app->inputs.sys_inputs = &app->state;
  app->inputs.sys_params = app->params;
  logi_debug("Creating GSL system");
  void *jac;
  switch (app->conf->sim_jacobian) {
  case LION_JACOBIAN_ANALYTICAL:
    jac = &lion_slv_jac_analytical;
    break;
  case LION_JACOBIAN_2POINT:
    jac = &lion_slv_jac_2point;
    break;
  default:
    jac = NULL;
    break;
  }
  gsl_odeiv2_system sys = {
    .function  = &lion_slv_system,
    .jacobian  = jac,
    .dimension = LION_SLV_DIMENSION,
    .params    = &app->inputs,
  };
  app->sys = sys;
  return LION_STATUS_SUCCESS;
}

lion_status_t _init_ode_driver(lion_app_t *app) {
  app->driver = gsl_odeiv2_driver_alloc_y_new(&app->sys, app->step_type, app->conf->sim_step_seconds, app->conf->sim_epsabs, app->conf->sim_epsrel);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_init(lion_app_t *app) {
  logi_debug("Configuring simulation stepper");
  LION_CALL_I(_init_simulation_stepper(app), "Failed initializing simulation stepper");

  logi_debug("Configuring optimization minimizer");
  LION_CALL_I(_init_simulation_minimizer(app), "Failed initializing simulation minimizer");

  logi_info("Configuring initial state");
  LION_CALL_I(_init_initial_state(app), "Failed initializing initial state");

  logi_info("Configuring ode system");
  LION_CALL_I(_init_ode_system(app), "Failed initializing ode system");

  logi_info("Configuring simulation driver");
  LION_CALL_I(_init_ode_driver(app), "Failed initializing ode driver");

  logi_debug("Showing initialization information");
  LION_CALL_I(lion_app_show_state_debug(app), "Failed showing initialization information");

  // Initialization hook
  if (app->init_hook != NULL) {
    logi_debug("Found init hook");
    LION_CALLDF_I(app->init_hook(app), "Failed calling init hook");
  }

  logi_info("Finished initialization");
  lion_app_log_startup_info(app);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_reset(lion_app_t *app) {
  logi_debug("Resetting simulator");
  LION_CALL_I(_init_initial_state(app), "Failed resetting initial state");
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_step(lion_app_t *app, double power, double ambient_temperature) {
  /*
     By using this update logic, at the end of every call app->state contains the inputs,
     outputs and states at timestep k, and the states at k+1 are stored in placeholder
     variables
  */

  // app->state = {x(k - 1), y(k - 1), u(k - 1)}
  app->state.soc_nominal          = app->state._next_soc_nominal;
  app->state.internal_temperature = app->state._next_internal_temperature;
  // app->state = {x(k), y(k - 1), u(k - 1)}
  app->state.power                = power;
  app->state.ambient_temperature  = ambient_temperature;
  // app->state = {x(k), y(k - 1), u(k)}
  LION_CALL_I(lion_slv_update(app), "Failed updating state");
  // app->state = {x(k), y(k), u(k)}
  double partial_result[2] = {app->state.soc_nominal, app->state.internal_temperature};
  LION_GSL_VCALL_I(
      gsl_odeiv2_driver_apply_fixed_step(app->driver, &app->state.time, app->conf->sim_step_seconds, 1, partial_result),
      "Failed at step %" PRIu64 " (t = %f)",
      app->state.step,
      app->state.time
  );
  app->state._next_soc_nominal          = partial_result[0];
  app->state._next_internal_temperature = partial_result[1];

  if (app->update_hook != NULL) {
    // TODO: Evaluate implementation of concurrency
    // TODO: Add some mechanism to avoid race conditions
    LION_CALLDF_I(app->update_hook(app), "Failed calling update hook");
  }
  app->state.step++;
  // TODO: Add time update
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_run(lion_app_t *app, lion_vector_t *power, lion_vector_t *ambient_temperature) {
  logi_info("Application start");
#ifndef NDEBUG
  if (app->_idebug_heap_head == NULL)
    LION_CALL_I(lion_app_init_debug(app), "Failed initializing debug information");
#endif

  if (power != NULL && ambient_temperature != NULL) {
    logi_info("Initializing application");
    LION_CALL_I(lion_app_init(app), "Failed initializing app");

    logi_debug("Running application");
    LION_CALL_I(lion_app_simulate(app, power, ambient_temperature), "Failed simulating system");
  } else {
    logi_error("Null arguments were passed, skipping application running");
  }

  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_cleanup(lion_app_t *app) {
  if (app->driver != NULL) {
    logi_info("GSL driver detected, freeing it");
    gsl_odeiv2_driver_free(app->driver);
  } else {
    logi_warn("No GSL driver detected");
  }

  if (app->sys_min != NULL) {
    logi_info("GSL minimizer detected, freeing it");
    gsl_min_fminimizer_free(app->sys_min);
  } else {
    logi_warn("No GSL minimizer detected");
  }

#ifndef NDEBUG
  if (app->_idebug_malloced_total != 0) {
    logi_warn("MEMORY LEAK: Found %lli elements (%d B) in heap after cleanup", app->_idebug_malloced_total, app->_idebug_malloced_size);

    logi_warn("MEMORY LEAK LOCATIONS:");
    _idebug_heap_info_t *node = app->_idebug_heap_head;
    while (node != NULL) {
      logi_warn(" * %#p (%d B) @ %s:%d", node->addr, node->size, node->file, node->line);
      node = node->next;
    }
  }

  _idebug_heap_info_t *node  = app->_idebug_heap_head;
  int64_t              count = (int64_t)heapinfo_count(app);
  if (app->_idebug_malloced_total != count) {
    logi_error("Found mismatch between reported (%d) and stored (%d) allocations", app->_idebug_malloced_total, count);

    node = app->_idebug_heap_head;
    logi_error("Stored allocations are:");
    while (node != NULL) {
      logi_error(" * %#p (%d B) @ %s:%d", node->addr, node->size, node->file, node->line);
      node = node->next;
    }
  }
  heapinfo_clean(app);
#endif

  return LION_STATUS_SUCCESS;
}

lion_version_t lion_app_get_version(lion_app_t *app) {
  lion_version_t out = {
    .major = LION_ENGINE_VERSION_MAJOR,
    .minor = LION_ENGINE_VERSION_MINOR,
    .patch = LION_ENGINE_VERSION_PATCH,
  };
  return out;
}

int lion_app_should_close(lion_app_t *app) { return 0; }

uint64_t lion_app_max_iters(lion_app_t *app) { return (uint64_t)(app->conf->sim_time_seconds / app->conf->sim_step_seconds); }

const char *lion_app_regime_name(lion_app_regime_t regime) {
  switch (regime) {
  case LION_APP_ONLYSF:
    return "LION_APP_ONLYSF";
  case LION_APP_ONLYAIR:
    return "LION_APP_ONLYAIR";
  case LION_APP_BOTH:
    return "LION_APP_BOTH";
  default:
    return "Regime not found";
  }
  return "Unexpected return";
}

const char *lion_app_stepper_name(lion_app_stepper_t stepper) {
  switch (stepper) {
  case LION_STEPPER_RK2:
    return "LION_STEPPER_RK2";
  case LION_STEPPER_RK4:
    return "LION_STEPPER_RK4";
  case LION_STEPPER_RKF45:
    return "LION_STEPPER_RKF45";
  case LION_STEPPER_RKCK:
    return "LION_STEPPER_RKCK";
  case LION_STEPPER_RK8PD:
    return "LION_STEPPER_RK8PD";
  case LION_STEPPER_RK1IMP:
    return "LION_STEPPER_RK1IMP";
  case LION_STEPPER_RK2IMP:
    return "LION_STEPPER_RK2IMP";
  case LION_STEPPER_RK4IMP:
    return "LION_STEPPER_RK4IMP";
  case LION_STEPPER_BSIMP:
    return "LION_STEPPER_BSIMP";
  case LION_STEPPER_MSADAMS:
    return "LION_STEPPER_MSADAMS";
  case LION_STEPPER_MSBDF:
    return "LION_STEPPER_MSBDF";
  default:
    return "Stepper not found";
  }
  return "Unexpected return";
}

const char *lion_app_minimizer_name(lion_app_minimizer_t minimizer) {
  switch (minimizer) {
  case LION_MINIMIZER_GOLDENSECTION:
    return "LION_MINIMIZER_GOLDENSECTION";
  case LION_MINIMIZER_BRENT:
    return "LION_MINIMIZER_BRENT";
  case LION_MINIMIZER_QUADGOLDEN:
    return "LION_MINIMIZER_QUADGOLDEN";
  default:
    return "Minimizer not found";
  }
  return "Unexpected return";
}

const char *lion_app_gsl_errno_name(const int num) { return gsl_strerror(num); }
