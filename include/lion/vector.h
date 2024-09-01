#pragma once

#include <stddef.h>
#include <stdint.h>

#include "status.h"

typedef struct lion_app lion_app_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lion_vector {
  void *data;
  size_t data_size;
  size_t len;
  size_t capacity;
} lion_vector_t;

/* Vector initialization */

lion_status_t lion_vector_new(lion_app_t *app, const size_t data_size,
                              lion_vector_t *out);
lion_status_t lion_vector_zero(lion_app_t *app, const size_t len,
                               const size_t data_size, lion_vector_t *out);
lion_status_t lion_vector_with_capacity(lion_app_t *app, const size_t capacity,
                                        const size_t data_size,
                                        lion_vector_t *out);
lion_status_t lion_vector_from_array(lion_app_t *app, const void *data,
                                     const size_t len, const size_t data_size,
                                     lion_vector_t *out);

/* Vector finalization */

lion_status_t lion_vector_cleanup(lion_app_t *app,
                                  const lion_vector_t *const vec);

/* Vector attributes */

lion_status_t lion_vector_get(lion_app_t *app, const lion_vector_t *vec,
                              const size_t i, void *out);
lion_status_t lion_vector_set(lion_app_t *app, lion_vector_t *vec,
                              const size_t i, const void *src);
lion_status_t lion_vector_resize(lion_app_t *app, lion_vector_t *vec,
                                 const size_t new_capacity);
lion_status_t lion_vector_push(lion_app_t *app, lion_vector_t *vec,
                               const void *src);
lion_status_t lion_vector_extend_array(lion_app_t *app, lion_vector_t *vec,
                                       const void *src, const size_t len);

size_t lion_vector_total_size(lion_app_t *app, const lion_vector_t *vec);
size_t lion_vector_alloc_size(lion_app_t *app, const lion_vector_t *vec);

#ifdef __cplusplus
}
#endif
