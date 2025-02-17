#include "mem.h"

#include <lion/sim.h>
#include <lion_utils/vendor/log.h>
#include <lionu/macros.h>
#include <stdlib.h>

#ifndef NDEBUG

// These functions only get defined and called if in debug mode

extern inline void *_lion_malloc(lion_sim_t *sim, size_t size, const char *filename, int line) {
  logi_debug("Allocating %i B in heap with 'malloc' @ %s:%d", size, filename, line);
  void *ret = malloc(size);
  if (ret != NULL && sim != NULL) {
    heapinfo_push(sim, ret, size, filename, line);
    sim->_idebug_malloced_total++;
    sim->_idebug_malloced_size += size;
    logi_debug("%d elements (%d B) in heap", sim->_idebug_malloced_total, sim->_idebug_malloced_size);
  }
  return ret;
}

extern inline void *_lion_realloc(lion_sim_t *sim, void *ptr, size_t new_size, const char *filename, int line) {
  logi_debug("Allocating %i B in heap with 'realloc' @ %s:%d", new_size, filename, line);
  if (ptr == NULL) {
    logi_debug("'realloc' will allocate brand new memory");
  }
  size_t addr = (size_t)ptr;
  void  *ret  = realloc(ptr, new_size);
  if (ret != NULL && sim != NULL) {
    if (addr != 0) {
      sim->_idebug_malloced_size -= heapinfo_popaddr(sim, (void *)addr);
    } else {
      sim->_idebug_malloced_total++;
    }
    heapinfo_push(sim, ret, new_size, filename, line);
    sim->_idebug_malloced_size += new_size;
    logi_debug("%d elements (%d B) in heap", sim->_idebug_malloced_total, sim->_idebug_malloced_size);
  }
  return ret;
}

extern inline void *_lion_calloc(lion_sim_t *sim, size_t num, size_t size, const char *filename, int line) {
  logi_debug("Allocating %ix(%i B) in heap with 'calloc' @ %s:%d", num, size, filename, line);
  void *ret = calloc(num, size);
  if (ret != NULL & sim != NULL) {
    heapinfo_push(sim, ret, num * size, filename, line);
    sim->_idebug_malloced_size += num * size;
    sim->_idebug_malloced_total++;
    logi_debug("%d elements (%d B) in heap", sim->_idebug_malloced_total, sim->_idebug_malloced_size);
  }
  return ret;
}

extern inline void _lion_free(lion_sim_t *sim, void *ptr, const char *filename, int line) {
  if (ptr != NULL && sim != NULL) {
    logi_debug("Freeing memory in heap at address %#p @ %s:%d", ptr, filename, line);
    size_t size                 = heapinfo_popaddr(sim, ptr);
    sim->_idebug_malloced_size -= size;
    sim->_idebug_malloced_total--;
    logi_debug("%d elements (%d B) in heap", sim->_idebug_malloced_total, sim->_idebug_malloced_size);
  }
  free(ptr);
}

#endif
