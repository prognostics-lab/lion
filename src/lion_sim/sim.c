#include "mem.h"
#include "sim_run.h"
#include "solver/sys.h"
#include "solver/update.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv2.h>
#include <inttypes.h>
#include <lion/lion.h>
#include <lion_math/dynamics/soh.h>
#include <lion_utils/macros.h>
#include <lion_utils/vendor/log.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

const lion_sim_config_t LION_SIM_CONFIG_DEFAULT = {
  // Metadata
  .sim_name = "Simulation",

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

lion_status_t lion_sim_config_new(lion_sim_config_t *out) {
  lion_sim_config_t conf = LION_SIM_CONFIG_DEFAULT;
  *out                   = conf;
  return LION_STATUS_SUCCESS;
}

lion_sim_config_t lion_sim_config_default(void) { return LION_SIM_CONFIG_DEFAULT; }

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

lion_status_t lion_sim_new(lion_sim_config_t *conf, lion_params_t *params, lion_sim_t *out) {
  lion_sim_t sim = {
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
  log_set_level(sim.conf->log_stdlvl);

  if (sim.conf->log_dir == NULL) {
    logi_warn("Log directory not specified, not logging to file");
  } else {
    if (create_directory(sim.conf->log_dir) == LION_STATUS_SUCCESS) {
      size_t log_dir_len = strnlen(sim.conf->log_dir, FILENAME_MAX);
      strncpy(sim.log_filename, sim.conf->log_dir, log_dir_len);

      sim.log_filename[strftime(sim.log_filename + log_dir_len, _LION_LOGFILE_MAX, "/%Y%m%d_%H%M%S.txt", time) + log_dir_len] = '\0';

      sim.log_file = fopen(sim.log_filename, "w");
      if (sim.log_file == NULL) {
        logi_error("Failed to create log file, not logging to file");
      } else {
        logi_info("Log file : '%s'", sim.log_filename);
        log_add_fp_internal(sim.log_file, sim.conf->log_filelvl);
      }
    } else {
      logi_error("Failed to create log directory, not logging to file");
    }
  }

#ifndef NDEBUG
  LION_CALL_I(lion_sim_init_debug(&sim), "Failed initializing debug information");
#endif
  *out = sim;
  return LION_STATUS_SUCCESS;
}

static void lion_sim_log_startup_info(lion_sim_t *sim) {
#ifndef NDEBUG
  logi_warn("!!! RUNNING IN DEBUG MODE !!!");
#endif
  logi_info("+-------------------------------------------------------+");
  logi_info("|################# STARTUP INFORMATION #################|");
  logi_info("+-------------------------------------------------------+");
  logi_info(" * Simulation name               : %s", sim->conf->sim_name);
#ifdef LION_ENGINE_VERSION_MAJOR
  lion_version_t ver = lion_sim_get_version(sim);
  logi_info(" * Engine version                 : %s.%s.%s", ver.major, ver.minor, ver.patch);
#else
  logi_info(" * Engine version                 : N/A");
#endif
  logi_info(" * `lion_sim_t` struct size       : %d B", sizeof(lion_sim_t));
  if (sim->conf->log_dir != NULL) {
    logi_info(" * Log directory                  : %s", sim->conf->log_dir);
    logi_info(" * Log std level                  : %d", sim->conf->log_stdlvl);
    logi_info(" * Log file level                 : %d", sim->conf->log_filelvl);
  }
  logi_info(" * Regime                         : %s", lion_regime_name(sim->conf->sim_regime));
  logi_info(" * Stepper                        : %s", lion_stepper_name(sim->conf->sim_stepper));
  logi_info(" * Minimizer                      : %s", lion_minimizer_name(sim->conf->sim_minimizer));
  logi_info(" * Jacobian                       : %s", lion_jacobian_name(sim->conf->sim_jacobian));
  logi_info(" * Total simulation time          : %f s", sim->conf->sim_time_seconds);
  logi_info(" * Simulation step time           : %f s", sim->conf->sim_step_seconds);
  logi_info(" * Absolute epsilon               : %f", sim->conf->sim_epsabs);
  logi_info(" * Relative epsilon               : %f", sim->conf->sim_epsrel);
  logi_info(" * Minimization max iterations    : %d iterations", sim->conf->sim_min_maxiter);
  if (sim->init_hook != NULL) {
    logi_info(" * Init hook                      : YES");
  } else {
    logi_info(" * Init hook                      : NO");
  }
  if (sim->update_hook != NULL) {
    logi_info(" * Update hook                    : YES");
  } else {
    logi_info(" * Update hook                    : NO");
  }
  if (sim->finished_hook != NULL) {
    logi_info(" * Finished hook                  : YES");
  } else {
    logi_info(" * Finished hook                  : NO");
  }
  logi_info(" * Initialization parameters");
  logi_info(" |-> State of charge              : %f %%", 100.0 * sim->params->init.soc);
  logi_info(" |-> State of health              : %f %%", 100.0 * sim->params->init.soh);
  logi_info(" |-> Internal temperature         : %f K", sim->params->init.temp_in);
  logi_info(" |-> Nominal capacity             : %f C (%f Ah)", sim->params->init.capacity, sim->params->init.capacity / 3600.0);
  logi_info(" |-> Current guess                : %f A", sim->params->init.current_guess);
  logi_info(" * Entropic heat coefficient parameters");
  logi_info(" |-> a                            : %f V/K", sim->params->ehc.a);
  logi_info(" |-> b                            : %f V/K", sim->params->ehc.b);
  logi_info(" |-> kappa                        : %f", sim->params->ehc.kappa);
  logi_info(" |-> mu                           : %f", sim->params->ehc.mu);
  logi_info(" |-> sigma                        : %f", sim->params->ehc.sigma);
  logi_info(" |-> l                            : %f", sim->params->ehc.l);
  logi_info(" * Open circuit voltage");
  logi_info(" |-> alpha                        : %f", sim->params->ocv.alpha);
  logi_info(" |-> beta                         : %f", sim->params->ocv.beta);
  logi_info(" |-> gamma                        : %f", sim->params->ocv.gamma);
  logi_info(" |-> v0                           : %f V", sim->params->ocv.v0);
  logi_info(" |-> vl                           : %f V", sim->params->ocv.vl);
  logi_info(" * Vogel-Fulcher-Tammann");
  logi_info(" |-> k1                           : %f K", sim->params->vft.k1);
  logi_info(" |-> k2                           : %f K", sim->params->vft.k2);
  logi_info(" |-> Reference temperature        : %f K", sim->params->vft.tref);
  logi_info(" * Temperature model");
  logi_info(" |-> Heat capacity                : %f J K-1", sim->params->temp.cp);
  logi_info(" |-> Internal thermal resistivity : %f K W-1", sim->params->temp.rin);
  logi_info(" |-> Outer thermal resistivity    : %f K W-1", sim->params->temp.rout);
  logi_info(" * Internal resistance model");
  logi_info(" |-> Model                        : %s", lion_params_rint_get_name(sim->params->rint.model));
  logi_info(" * Degradation model");
  logi_info(" |-> Total cycles                 : %" PRIu64 " cycles", sim->params->soh.total_cycles);
  logi_info(" |-> SoH at EoL                   : %f %%", 100.0 * sim->params->soh.final_soh);
  logi_info("+-------------------------------------------------------+");
  logi_info("|################# END OF INFORMATION ##################|");
  logi_info("+-------------------------------------------------------+");
}

lion_status_t _init_simulation_stepper(lion_sim_t *sim) {
  switch (sim->conf->sim_stepper) {
  case LION_STEPPER_RK2:
    sim->step_type = gsl_odeiv2_step_rk2;
    break;
  case LION_STEPPER_RK4:
    sim->step_type = gsl_odeiv2_step_rk4;
    break;
  case LION_STEPPER_RKF45:
    sim->step_type = gsl_odeiv2_step_rkf45;
    break;
  case LION_STEPPER_RKCK:
    sim->step_type = gsl_odeiv2_step_rkck;
    break;
  case LION_STEPPER_RK8PD:
    sim->step_type = gsl_odeiv2_step_rk8pd;
    break;
  case LION_STEPPER_RK1IMP:
    sim->step_type = gsl_odeiv2_step_rk1imp;
    break;
  case LION_STEPPER_RK2IMP:
    sim->step_type = gsl_odeiv2_step_rk2imp;
    break;
  case LION_STEPPER_RK4IMP:
    sim->step_type = gsl_odeiv2_step_rk4imp;
    break;
  case LION_STEPPER_BSIMP:
    sim->step_type = gsl_odeiv2_step_bsimp;
    break;
  case LION_STEPPER_MSADAMS:
    sim->step_type = gsl_odeiv2_step_msadams;
    break;
  case LION_STEPPER_MSBDF:
    sim->step_type = gsl_odeiv2_step_msbdf;
    break;
  default:
    logi_error("Desired step type not implemented");
    return LION_STATUS_FAILURE;
  }
  return LION_STATUS_SUCCESS;
}

lion_status_t _init_simulation_minimizer(lion_sim_t *sim) {
  switch (sim->conf->sim_minimizer) {
  case LION_MINIMIZER_GOLDENSECTION:
    sim->minimizer = gsl_min_fminimizer_goldensection;
    break;
  case LION_MINIMIZER_BRENT:
    sim->minimizer = gsl_min_fminimizer_brent;
    break;
  case LION_MINIMIZER_QUADGOLDEN:
    sim->minimizer = gsl_min_fminimizer_quad_golden;
    break;
  default:
    logi_error("Desired minimizer not implemented");
    return LION_STATUS_FAILURE;
  }
  sim->sys_min = gsl_min_fminimizer_alloc(sim->minimizer);
  logi_info("Using minimizer %s", gsl_min_fminimizer_name(sim->sys_min));
  return LION_STATUS_SUCCESS;
}

lion_status_t _init_initial_state(lion_sim_t *sim) {
  logi_debug("Setting up initial conditions");
  // The current is set at first because it is used as an initial guess
  // for the optimization problem
  sim->state._next_soc_nominal          = sim->params->init.soc;
  sim->state._next_internal_temperature = sim->params->init.temp_in;
  sim->state._acc_discharge             = 0.0;
  sim->state._soc_mean                  = 0.0;
  sim->state._soc_max                   = 0.0;
  sim->state._soc_min                   = 1.0;
  sim->state.soh                        = sim->params->init.soh;
  sim->state.current                    = sim->params->init.current_guess;
  sim->state.time                       = 0.0;
  sim->state.step                       = 0;
  sim->state.cycle                      = 0;
  return LION_STATUS_SUCCESS;
}

lion_status_t _init_ode_system(lion_sim_t *sim) {
  logi_debug("Setting up GSL inputs");
  sim->inputs.sys_inputs = &sim->state;
  sim->inputs.sys_params = sim->params;
  logi_debug("Creating GSL system");
  void *jac;
  switch (sim->conf->sim_jacobian) {
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
    .function  = &lion_slv_system_continuous,
    .jacobian  = jac,
    .dimension = LION_SLV_DIMENSION,
    .params    = &sim->inputs,
  };
  sim->sys = sys;
  return LION_STATUS_SUCCESS;
}

lion_status_t _init_ode_driver(lion_sim_t *sim) {
  sim->driver = gsl_odeiv2_driver_alloc_y_new(&sim->sys, sim->step_type, sim->conf->sim_step_seconds, sim->conf->sim_epsabs, sim->conf->sim_epsrel);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_sim_init(lion_sim_t *sim) {
  logi_debug("Configuring simulation stepper");
  LION_CALL_I(_init_simulation_stepper(sim), "Failed initializing simulation stepper");

  logi_debug("Configuring optimization minimizer");
  LION_CALL_I(_init_simulation_minimizer(sim), "Failed initializing simulation minimizer");

  logi_info("Configuring initial state");
  LION_CALL_I(_init_initial_state(sim), "Failed initializing initial state");

  logi_info("Configuring ode system");
  LION_CALL_I(_init_ode_system(sim), "Failed initializing ode system");

  logi_info("Configuring simulation driver");
  LION_CALL_I(_init_ode_driver(sim), "Failed initializing ode driver");

  logi_debug("Showing initialization information");
  LION_CALL_I(lion_sim_show_state_debug(sim), "Failed showing initialization information");

  // Initialization hook
  if (sim->init_hook != NULL) {
    logi_debug("Found init hook");
    LION_CALLDF_I(sim->init_hook(sim), "Failed calling init hook");
  }

  logi_info("Finished initialization");
  lion_sim_log_startup_info(sim);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_sim_reset(lion_sim_t *sim) {
  logi_debug("Resetting simulator");
  LION_CALL_I(_init_initial_state(sim), "Failed resetting initial state");
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_sim_step(lion_sim_t *sim, double power, double ambient_temperature) {
  /*
     By using this update logic, at the end of every call sim->state contains the inputs,
     outputs and states at timestep k, and the states at k+1 are stored in placeholder
     variables
  */

  // sim->state = {x(k - 1), y(k - 1), u(k - 1)}
  sim->state.soc_nominal          = sim->state._next_soc_nominal;
  sim->state.internal_temperature = sim->state._next_internal_temperature;
  // sim->state = {x(k), y(k - 1), u(k - 1)}
  sim->state.power                = power;
  sim->state.ambient_temperature  = ambient_temperature;
  // sim->state = {x(k), y(k - 1), u(k)}
  LION_CALL_I(lion_slv_update(sim), "Failed updating state");
  // sim->state = {x(k), y(k), u(k)}
  double partial_result[2] = {sim->state.soc_nominal, sim->state.internal_temperature};
  LION_GSL_VCALL_I(
      gsl_odeiv2_driver_apply_fixed_step(sim->driver, &sim->state.time, sim->conf->sim_step_seconds, 1, partial_result),
      "Failed at step %" PRIu64 " (t = %f)",
      sim->state.step,
      sim->state.time
  );
  sim->state._next_soc_nominal          = partial_result[0];
  sim->state._next_internal_temperature = partial_result[1];

  // Update SoC statistics
  sim->state._soc_mean = ((double)sim->state._cycle_step * sim->state._soc_mean + sim->state.soc_nominal) / (double)(sim->state._cycle_step + 1);
  if (sim->state.soc_nominal > sim->state._soc_max)
    sim->state._soc_max = sim->state.soc_nominal;
  if (sim->state.soc_nominal < sim->state._soc_min)
    sim->state._soc_min = sim->state.soc_nominal;

  // Update the degradation state of the cell
  sim->state._acc_discharge += GSL_MAX_DBL(sim->state.current * sim->conf->sim_step_seconds, 0.0);
  if (sim->state._acc_discharge >= sim->state.capacity_nominal) {
    // A cycle has been completed so we update the SoH
    sim->state._acc_discharge = fmod(sim->state._acc_discharge, sim->state.capacity_nominal);
    sim->state.soh            = lion_soh_next(sim->state.soh, sim->state._soc_mean, sim->state._soc_max, sim->state._soc_min, sim->params);

    // Restart placeholder values
    sim->state._soc_mean   = 0.0;
    sim->state._soc_max    = 0.0;
    sim->state._soc_min    = 1.0;
    sim->state._cycle_step = 0;

    sim->state.cycle++;
  } else {
    sim->state._cycle_step++;
  }

  if (sim->update_hook != NULL) {
    // TODO: Evaluate implementation of concurrency
    // TODO: Add some mechanism to avoid race conditions
    LION_CALLDF_I(sim->update_hook(sim), "Failed calling update hook");
  }
  sim->state.step++;
  // TODO: Add time update
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_sim_run(lion_sim_t *sim, lion_vector_t *power, lion_vector_t *ambient_temperature) {
  logi_info("Simulation start");
#ifndef NDEBUG
  if (sim->_idebug_heap_head == NULL)
    LION_CALL_I(lion_sim_init_debug(sim), "Failed initializing debug information");
#endif

  if (power != NULL && ambient_temperature != NULL) {
    logi_info("Initializing simulation");
    LION_CALL_I(lion_sim_init(sim), "Failed initializing sim");

    logi_debug("Running simulation");
    LION_CALL_I(lion_sim_simulate(sim, power, ambient_temperature), "Failed simulating system");
  } else {
    logi_error("Null arguments were passed, skipping simulation running");
  }

  return LION_STATUS_SUCCESS;
}

lion_status_t lion_sim_cleanup(lion_sim_t *sim) {
  if (sim->driver != NULL) {
    logi_info("GSL driver detected, freeing it");
    gsl_odeiv2_driver_free(sim->driver);
  } else {
    logi_warn("No GSL driver detected");
  }

  if (sim->sys_min != NULL) {
    logi_info("GSL minimizer detected, freeing it");
    gsl_min_fminimizer_free(sim->sys_min);
  } else {
    logi_warn("No GSL minimizer detected");
  }

#ifndef NDEBUG
  if (sim->_idebug_malloced_total != 0) {
    logi_warn("MEMORY LEAK: Found %lli elements (%d B) in heap after cleanup", sim->_idebug_malloced_total, sim->_idebug_malloced_size);

    logi_warn("MEMORY LEAK LOCATIONS:");
    _idebug_heap_info_t *node = sim->_idebug_heap_head;
    while (node != NULL) {
      logi_warn(" * %#p (%d B) @ %s:%d", node->addr, node->size, node->file, node->line);
      node = node->next;
    }
  }

  _idebug_heap_info_t *node  = sim->_idebug_heap_head;
  int64_t              count = (int64_t)heapinfo_count(sim);
  if (sim->_idebug_malloced_total != count) {
    logi_error("Found mismatch between reported (%d) and stored (%d) allocations", sim->_idebug_malloced_total, count);

    node = sim->_idebug_heap_head;
    logi_error("Stored allocations are:");
    while (node != NULL) {
      logi_error(" * %#p (%d B) @ %s:%d", node->addr, node->size, node->file, node->line);
      node = node->next;
    }
  }
  heapinfo_clean(sim);
#endif

  return LION_STATUS_SUCCESS;
}

lion_version_t lion_sim_get_version(lion_sim_t *sim) {
  lion_version_t out = {
    .major = LION_ENGINE_VERSION_MAJOR,
    .minor = LION_ENGINE_VERSION_MINOR,
    .patch = LION_ENGINE_VERSION_PATCH,
  };
  return out;
}

int lion_sim_should_close(lion_sim_t *sim) { return 0; }

uint64_t lion_sim_max_iters(lion_sim_t *sim) { return (uint64_t)(sim->conf->sim_time_seconds / sim->conf->sim_step_seconds); }
