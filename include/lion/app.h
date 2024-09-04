#pragma once

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <gsl/gsl_odeiv2.h>

#include "params.h"
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

typedef enum terra_app_regimes {
  TERRA_APP_ONLYSF = 0,
  TERRA_APP_ONLYAIR = 1,
  TERRA_APP_BOTH = 2,
} terra_app_regimes_t;

typedef enum terra_app_steppers {
  TERRA_STEPPER_RK2 = 0,
  TERRA_STEPPER_RK4 = 1,
  TERRA_STEPPER_RKF45 = 2,
  TERRA_STEPPER_RKCK = 3,
  TERRA_STEPPER_RK8PD = 4,
  TERRA_STEPPER_RK1IMP = 5,
  TERRA_STEPPER_RK2IMP = 6,
  TERRA_STEPPER_RK4IMP = 7,
  TERRA_STEPPER_BSIMP = 8,
  TERRA_STEPPER_MSADAMS = 9,
  TERRA_STEPPER_MSBDF = 10,
} terra_app_steppers_t;

typedef struct terra_app_config {
  /* App metadata */

  const char *app_name;

  /* Simulation metadata */

  terra_app_regimes_t sim_regime;
  terra_app_steppers_t sim_stepper;
  double sim_time_seconds;
  double sim_step_seconds;
  double sim_epsabs;
  double sim_epsrel;

  /* Logging configuration */

  const char *log_dir;
  int log_stdlvl;
  int log_filelvl;
} lion_app_config_t;

typedef struct lion_app {
  lion_app_config_t *conf;
  lion_params_t *params;

  /* Data handles */

  gsl_odeiv2_system sys;
  gsl_odeiv2_driver *driver;
  gsl_odeiv2_step_type *step_type;

  char log_filename[FILENAME_MAX + _LION_LOGFILE_MAX];
  FILE *log_file;

#ifndef NDEBUG
  /* Internal debug information */

  int64_t _idebug_malloced_total;
  size_t _idebug_malloced_size;
  _idebug_heap_info_t *_idebug_heap_head;
#endif
} lion_app_t;

// Constants

extern const lion_app_config_t LION_APP_CONFIG_DEFAULT;

// Functions

lion_status_t lion_app_config_new(lion_app_config_t *out);
lion_app_config_t lion_app_config_default(void);
lion_params_t lion_app_params_default(void);

lion_status_t lion_app_new(lion_app_config_t *conf, lion_params_t *params,
                           lion_app_t *out);
lion_status_t lion_app_run(lion_app_t *app);

int lion_app_should_close(lion_app_t *app);

lion_status_t lion_app_cleanup(lion_app_t *app);

#ifdef __cplusplus
}
#endif
