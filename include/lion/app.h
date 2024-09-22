#pragma once

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <gsl/gsl_min.h>
#include <gsl/gsl_odeiv2.h>

#include "params.h"
#include "solver/inputs.h"
#include "status.h"
#include "vector.h"

#define LION_ENGINE_VERSION_MAJOR 0
#define LION_ENGINE_VERSION_MINOR 1
#define LION_ENGINE_VERSION_PATCH 0

#define _LION_LOGFILE_MAX 64

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations

typedef struct lion_app lion_app_t;

// Debug declarations

#ifndef NDEBUG
typedef struct _idebug_heap_info {
  void *addr;
  size_t size;
  char file[FILENAME_MAX];
  int line;
  struct _idebug_heap_info *next;
} _idebug_heap_info_t;

_idebug_heap_info_t *heapinfo_new(lion_app_t *app);
void heapinfo_free_node(_idebug_heap_info_t *node);
void heapinfo_clean(lion_app_t *app);
void heapinfo_push(lion_app_t *app, void *addr, size_t size, const char *file,
                   int line);
size_t heapinfo_popaddr(lion_app_t *app, void *addr);
size_t heapinfo_count(lion_app_t *app);
#endif

// Application declarations

typedef enum lion_app_regime {
  LION_APP_ONLYSF = 0,
  LION_APP_ONLYAIR = 1,
  LION_APP_BOTH = 2,
} lion_app_regime_t;

typedef enum lion_app_stepper {
  LION_STEPPER_RK2 = 0,
  LION_STEPPER_RK4 = 1,
  LION_STEPPER_RKF45 = 2,
  LION_STEPPER_RKCK = 3,
  LION_STEPPER_RK8PD = 4,
  LION_STEPPER_RK1IMP = 5,
  LION_STEPPER_RK2IMP = 6,
  LION_STEPPER_RK4IMP = 7,
  LION_STEPPER_BSIMP = 8,
  LION_STEPPER_MSADAMS = 9,
  LION_STEPPER_MSBDF = 10,
} lion_app_stepper_t;

typedef enum lion_app_minimizer {
  LION_MINIMIZER_GOLDENSECTION = 0,
  LION_MINIMIZER_BRENT = 1,
  LION_MINIMIZER_QUADGOLDEN = 2,
} lion_app_minimizer_t;

typedef struct lion_app_config {
  /* App metadata */

  const char *app_name;

  /* Simulation metadata */

  lion_app_regime_t sim_regime;
  lion_app_stepper_t sim_stepper;
  lion_app_minimizer_t sim_minimizer;
  double sim_time_seconds;
  double sim_step_seconds;
  double sim_epsabs;
  double sim_epsrel;
  uint64_t sim_min_maxiter;

  /* Logging configuration */

  const char *log_dir;
  int log_stdlvl;
  int log_filelvl;
} lion_app_config_t;

typedef struct lion_app_state {
  double time;
  uint64_t step;

  // System inputs
  double power;
  double ambient_temperature;

  // Electrical state
  double voltage;
  double current;
  double open_circuit_voltage;
  double internal_resistance;

  // Thermal state
  double ehc;
  double generated_heat;
  double internal_temperature;
  double surface_temperature;

  // Charge state
  double kappa;
  double soc_nominal;
  double capacity_nominal;
  double soc_use;
  double capacity_use;
} lion_app_state_t;

typedef struct lion_app {
  lion_app_config_t *conf;
  lion_params_t *params;
  lion_app_state_t state;
  lion_slv_inputs_t inputs;
  lion_status_t (*init_hook)(lion_app_t *app);
  lion_status_t (*update_hook)(lion_app_t *app);
  lion_status_t (*finished_hook)(lion_app_t *app);

  /* Data handles */

  gsl_odeiv2_system sys;
  gsl_odeiv2_driver *driver;
  gsl_min_fminimizer *sys_min;
  const gsl_odeiv2_step_type *step_type;
  const gsl_min_fminimizer_type *minimizer;

  char log_filename[FILENAME_MAX + _LION_LOGFILE_MAX];
  FILE *log_file;

#ifndef NDEBUG
  /* Internal debug information */

  int64_t _idebug_malloced_total;
  size_t _idebug_malloced_size;
  _idebug_heap_info_t *_idebug_heap_head;
#endif
} lion_app_t;

lion_status_t lion_app_config_new(lion_app_config_t *out);
lion_app_config_t lion_app_config_default(void);

lion_status_t lion_app_new(lion_app_config_t *conf, lion_params_t *params,
                           lion_app_t *out);
lion_status_t lion_app_init(lion_app_t *app, double initial_power,
                            double initial_amb_temp);
lion_status_t lion_app_step(lion_app_t *app, double power,
                            double ambient_temperature);
lion_status_t lion_app_run(lion_app_t *app, lion_vector_t *power,
                           lion_vector_t *ambient_temperature);

int lion_app_should_close(lion_app_t *app);
uint64_t lion_app_max_iters(lion_app_t *app);

lion_status_t lion_app_cleanup(lion_app_t *app);

const char *lion_app_regime_name(lion_app_regime_t regime);
const char *lion_app_stepper_name(lion_app_stepper_t stepper);
const char *lion_app_minimizer_name(lion_app_minimizer_t minimizer);

#ifdef __cplusplus
}
#endif
