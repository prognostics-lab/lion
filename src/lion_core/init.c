#include "init.h"
#include <lion/lion.h>
#include <lion_utils/vendor/log.h>

lion_status_t lion_init(lion_app_t *app) {
  // TODO: Implement initialization logic
  return LION_STATUS_SUCCESS;
}

#ifndef NDEBUG
lion_status_t lion_init_debug(lion_app_t *app) {
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
