#pragma once

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <lionm/params.h>

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

typedef struct terra_app_config {
  /* App metadata */

  const char *app_name;

  /* Logging configuration */

  const char *log_dir;
  int log_stdlvl;
  int log_filelvl;
} lion_app_config_t;

typedef struct lion_app {
  lion_app_config_t *conf;
  lionm_params_t *params;

  /* Data handles */

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
lion_app_config_t terra_app_config_default(void);

lion_status_t lion_app_new(lion_app_config_t *conf, lion_app_t *out);
lion_status_t lion_app_run(lion_app_t *app);

int lion_app_should_close(lion_app_t *app);

lion_status_t lion_app_cleanup(lion_app_t *app);

#ifdef __cplusplus
}
#endif
