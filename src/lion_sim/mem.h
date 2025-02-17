#pragma once

#include <lion/sim.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG

  #define lion_malloc(sim, x)     malloc(x)
  #define lion_realloc(sim, x, y) realloc(x, y)
  #define lion_calloc(sim, x, y)  calloc(x, y)
  #define lion_free(sim, x)       free(x)

#else

void *_lion_malloc(lion_sim_t *sim, size_t size, const char *filename, int line);
void *_lion_realloc(lion_sim_t *sim, void *ptr, size_t new_size, const char *filename, int line);
void *_lion_calloc(lion_sim_t *sim, size_t num, size_t size, const char *filename, int line);
void  _lion_free(lion_sim_t *sim, void *ptr, const char *filename, int line);

  #define lion_malloc(sim, x)     _lion_malloc(sim, x, __FILENAME__, __LINE__)
  #define lion_realloc(sim, x, y) _lion_realloc(sim, x, y, __FILENAME__, __LINE__)
  #define lion_calloc(sim, x, y)  _lion_calloc(sim, x, y, __FILENAME__, __LINE__)
  #define lion_free(sim, x)       _lion_free(sim, x, __FILENAME__, __LINE__)

#endif

#ifdef __cplusplus
}
#endif
