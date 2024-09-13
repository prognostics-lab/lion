#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <gsl/gsl_odeiv2.h>

#include <lion/lion.h>
#include <lion_utils/macros.h>
#include <lion_utils/vendor/log.h>
#include <lionu/mem.h>

#include "app_run.h"

#ifdef _WIN32
#include <windows.h>
#define CREATE_DIRECTORY(dirname) CreateDirectory(dirname, NULL)
#define DIR_ALREADY_EXISTS_ERROR_CODE ERROR_ALREADY_EXISTS
#define EPOCH_DIFF 11644473600000ULL // Milliseconds
#define GET_CURRENT_TIME(current_time)                                         \
  {                                                                            \
    FILETIME ft;                                                               \
    ULARGE_INTEGER ft64;                                                       \
    GetSystemTimeAsFileTime(&ft);                                              \
    ft64.LowPart = ft.dwLowDateTime;                                           \
    ft64.HighPart = ft.dwHighDateTime;                                         \
    current_time = (int64_t)(ft64.QuadPart / 10000 - EPOCH_DIFF);              \
  }
#else
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#define CREATE_DIRECTORY(dirname) mkdir(dirname, 0777)
#define DIR_ALREADY_EXISTS_ERROR_CODE EEXIST
#define GET_CURRENT_TIME(current_time)                                         \
  {                                                                            \
    struct timeval tv;                                                         \
    gettimeofday(&tv, NULL);                                                   \
    current_time = (int64_t)tv.tv_sec * 1000LL + (int64_t)tv.tv_usec / 1000LL; \
  }
#endif

const lion_app_config_t LION_APP_CONFIG_DEFAULT = {
    // Metadata
    .app_name = "Application",
    .init_hook = NULL,
    .update_hook = NULL,
    .finished_hook = NULL,

    // Simulation parameters
    .sim_stepper = LION_STEPPER_RK8PD,
    .sim_minimizer = LION_MINIMIZER_BRENT,
    .sim_time_seconds = 10.0,
    .sim_step_seconds = 1e-3,
    .sim_epsabs = 1e-8,
    .sim_epsrel = 1e-8,

    // Logging
    .log_dir = NULL,
    .log_stdlvl = LOG_INFO,
    .log_filelvl = LOG_TRACE,
};

lion_status_t lion_app_config_new(lion_app_config_t *out) {
  lion_app_config_t conf = LION_APP_CONFIG_DEFAULT;
  *out = conf;
  return LION_STATUS_SUCCESS;
}

lion_app_config_t lion_app_config_default(void) {
  return LION_APP_CONFIG_DEFAULT;
}

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

lion_status_t lion_app_new(lion_app_config_t *conf, lion_params_t *params,
                           lion_app_t *out) {
  lion_app_t app = {
      .conf = conf,
      .params = params,

      .driver = NULL,
      .sys_min = NULL,
      .step_type = NULL,
      .minimizer = NULL,

#ifndef NDEBUG // Internal debug information
      ._idebug_malloced_total = 0,
#endif
  };

  // Logging setup
  time_t seconds = time(NULL);
  struct tm *time = localtime(&seconds);
  log_set_level(app.conf->log_stdlvl);

  if (app.conf->log_dir == NULL) {
    logi_warn("Log directory not specified, not logging to file");
  } else {
    if (create_directory(app.conf->log_dir) == LION_STATUS_SUCCESS) {
      size_t log_dir_len = strnlen(app.conf->log_dir, FILENAME_MAX);
      strncpy(app.log_filename, app.conf->log_dir, log_dir_len);

      app.log_filename[strftime(app.log_filename + log_dir_len,
                                _LION_LOGFILE_MAX, "/%Y%m%d_%H%M%S.txt", time) +
                       log_dir_len] = '\0';

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
  LION_CALL_I(lion_app_init_debug(&app),
              "Failed initializing debug information");
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
  logi_info(" * Engine version                 : %u.%u.%u",
            LION_ENGINE_VERSION_MAJOR, LION_ENGINE_VERSION_MINOR,
            LION_ENGINE_VERSION_PATCH);
  logi_info(" * `lion_app_t` struct size       : %d B", sizeof(lion_app_t));
  if (app->conf->log_dir != NULL) {
    logi_info(" * Log directory                  : %s", app->conf->log_dir);
    logi_info(" * Log std level                  : %d", app->conf->log_stdlvl);
    logi_info(" * Log file level                 : %d", app->conf->log_filelvl);
  }
  logi_info(" * Regime                         : %s",
            lion_app_regime_name(app->conf->sim_regime));
  logi_info(" * Stepper                        : %s",
            lion_app_stepper_name(app->conf->sim_stepper));
  logi_info(" * Minimizer                      : %s",
            lion_app_minimizer_name(app->conf->sim_minimizer));
  logi_info(" * Total simulation time          : %f s",
            app->conf->sim_time_seconds);
  logi_info(" * Simulation step time           : %f s",
            app->conf->sim_step_seconds);
  logi_info(" * Absolute epsilon               : %f", app->conf->sim_epsabs);
  logi_info(" * Relative epsilon               : %f", app->conf->sim_epsrel);
  logi_info(" * Minimization max iterations    : %d iterations",
            app->conf->sim_min_max_iter);
  if (app->conf->init_hook != NULL) {
    logi_info(" * Init hook                      : YES");
  } else {
    logi_info(" * Init hook                      : NO");
  }
  if (app->conf->update_hook != NULL) {
    logi_info(" * Update hook                    : YES");
  } else {
    logi_info(" * Update hook                    : NO");
  }
  if (app->conf->finished_hook != NULL) {
    logi_info(" * Finished hook                  : YES");
  } else {
    logi_info(" * Finished hook                  : NO");
  }
  // TODO: Add missing units of measurement
  logi_info(" * Initialization parameters");
  logi_info(" |-> State of charge              : %f %%",
            100.0 * app->params->init.initial_soc);
  logi_info(" |-> State of health              : %f %%",
            100.0 * app->params->init.initial_soh);
  logi_info(" |-> Internal temperature         : %f K",
            app->params->init.initial_internal_temperature);
  logi_info(" |-> Nominal capacity             : %f C (%f Ah)",
            app->params->init.initial_capacity,
            app->params->init.initial_capacity / 3600.0);
  logi_info(" |-> Current guess                : %f A",
            app->params->init.initial_current_guess);
  logi_info(" * Entropic heat coefficient parameters");
  logi_info(" |-> a                            : %f", app->params->ehc.a);
  logi_info(" |-> b                            : %f", app->params->ehc.b);
  logi_info(" |-> kappa                        : %f", app->params->ehc.kappa);
  logi_info(" |-> mu                           : %f", app->params->ehc.mu);
  logi_info(" |-> sigma                        : %f", app->params->ehc.sigma);
  logi_info(" |-> lambda                       : %f", app->params->ehc.lambda);
  logi_info(" * Open circuit voltage");
  logi_info(" |-> alpha                        : %f", app->params->ocv.alpha);
  logi_info(" |-> beta                         : %f", app->params->ocv.beta);
  logi_info(" |-> gamma                        : %f", app->params->ocv.gamma);
  logi_info(" |-> v0                           : %f", app->params->ocv.v0);
  logi_info(" |-> vl                           : %f", app->params->ocv.vl);
  logi_info(" * Vogel-Fulcher-Tammann");
  logi_info(" |-> k1                           : %f", app->params->vft.k1);
  logi_info(" |-> k2                           : %f", app->params->vft.k2);
  logi_info(" |-> Reference temperature        : %f K", app->params->vft.tref);
  logi_info(" * Temperature model");
  logi_info(" |-> Heat capacity                : %f J K-1", app->params->t.cp);
  logi_info(" |-> Internal thermal resistivity : %f K W-1", app->params->t.rin);
  logi_info(" |-> Outter thermal resistivity   : %f K W-1",
            app->params->t.rout);
  logi_info("+-------------------------------------------------------+");
  logi_info("|################# END OF INFORMATION ##################|");
  logi_info("+-------------------------------------------------------+");
}

lion_status_t lion_app_step(lion_app_t *app, double power,
                            double ambient_temperature) {
  // app->state contains state(k)
  double partial_result[2];
  LION_GSL_VCALL_I(gsl_odeiv2_driver_apply_fixed_step(
                       app->driver, &app->state.time,
                       app->conf->sim_step_seconds, 1, partial_result),
                   "Failed at step %d (t = %f)", i, app->state.time);
  // state(k + 1) is stored in partial_result
  app->state.soc_nominal = partial_result[0];
  app->state.internal_temperature = partial_result[1];
  app->state.power = power;
  app->state.ambient_temperature = ambient_temperature;
  LION_CALL_I(lion_slv_update(app), "Failed updating state");
  // partial results are spread over the state, meaning at this
  // point app->state contains state(k + 1) leaving it ready for the
  // next time iteration

  if (app->conf->update_hook != NULL) {
    // TODO: Evaluate implementation of concurrency
    // TODO: Add some mechanism to avoid race conditions
    LION_CALLDF_I(app->conf->update_hook(app), "Failed calling update hook");
  }
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_app_run(lion_app_t *app, lion_vector_t *power,
                           lion_vector_t *ambient_temperature) {
  logi_info("Application start");
  lion_app_log_startup_info(app);
#ifndef NDEBUG
  if (app->_idebug_heap_head == NULL)
    LION_CALL_I(lion_app_init_debug(app),
                "Failed initializing debug information");
#endif

  if (power != NULL && ambient_temperature != NULL) {
    logi_info("Initializing application");
    LION_CALL_I(lion_app_init(app, lion_vector_get_d(app, power, 0),
                              lion_vector_get_d(app, ambient_temperature, 0)),
                "Failed initializing app");

    logi_debug("Running application");
    LION_CALL_I(lion_app_simulate(app, power, ambient_temperature),
                "Failed simulating system");
  } else {
    logi_error("Null arguments were passed, skipping application running");
  }

  logi_debug("Cleaning up application");
  LION_CALL_I(lion_app_cleanup(app), "Failed cleaning app");
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
    logi_warn("MEMORY LEAK: Found %lli elements (%d B) in heap after cleanup",
              app->_idebug_malloced_total, app->_idebug_malloced_size);

    logi_warn("MEMORY LEAK LOCATIONS:");
    _idebug_heap_info_t *node = app->_idebug_heap_head;
    while (node != NULL) {
      logi_warn(" * %#p (%d B) @ %s:%d", node->addr, node->size, node->file,
                node->line);
      node = node->next;
    }
  }

  _idebug_heap_info_t *node = app->_idebug_heap_head;
  int64_t count = (int64_t)heapinfo_count(app);
  if (app->_idebug_malloced_total != count) {
    logi_error(
        "Found mismatch between reported (%d) and stored (%d) allocations",
        app->_idebug_malloced_total, count);

    node = app->_idebug_heap_head;
    logi_error("Stored allocations are:");
    while (node != NULL) {
      logi_error(" * %#p (%d B) @ %s:%d", node->addr, node->size, node->file,
                 node->line);
      node = node->next;
    }
  }
  heapinfo_clean(app);
#endif

  return LION_STATUS_SUCCESS;
}

int lion_app_should_close(lion_app_t *app) { return 0; }

uint64_t lion_app_max_iters(lion_app_t *app) {
  return (uint64_t)(app->conf->sim_time_seconds / app->conf->sim_step_seconds);
}

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
