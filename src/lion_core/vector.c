#include <lion/lion.h>
#include <lion_utils/vendor/log.h>
#include <lionu/mem.h>
#include <string.h>

lion_status_t lion_vector_new(lion_app_t *app, const size_t data_size,
                              lion_vector_t *out) {
  lion_vector_t result = {
      .data = NULL,
      .data_size = data_size,
      .len = 0,
      .capacity = 0,
  };
  *out = result;
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_vector_zero(lion_app_t *app, const size_t len,
                               const size_t data_size, lion_vector_t *out) {
  void *data = lionu_calloc(app, len, data_size);
  if (data == NULL) {
    return LION_STATUS_FAILURE;
  }

  lion_vector_t result = {
      .data = data,
      .data_size = data_size,
      .len = len,
      .capacity = len,
  };
  *out = result;
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_vector_with_capacity(lion_app_t *app, const size_t capacity,
                                        const size_t data_size,
                                        lion_vector_t *out) {
  void *data = lionu_malloc(app, data_size * capacity);
  if (data == NULL) {
    return LION_STATUS_FAILURE;
  }

  lion_vector_t result = {
      .data = data,
      .data_size = data_size,
      .len = 0,
      .capacity = capacity,
  };
  *out = result;
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_vector_from_array(lion_app_t *app, const void *data,
                                     const size_t len, const size_t data_size,
                                     lion_vector_t *out) {
  void *new_data = lionu_malloc(app, len * data_size);
  if (new_data == NULL) {
    return LION_STATUS_FAILURE;
  }

  memcpy(new_data, data, len * data_size);
  lion_vector_t result = {
      .data = new_data,
      .data_size = data_size,
      .len = len,
      .capacity = len,
  };
  *out = result;
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_vector_cleanup(lion_app_t *app,
                                  const lion_vector_t *const vec) {
  lionu_free(app, vec->data);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_vector_get(lion_app_t *app, const lion_vector_t *vec,
                              const size_t i, void *out) {
  if (vec->data == NULL || i >= vec->len || out == NULL) {
    return LION_STATUS_FAILURE;
  }
  memcpy(out, (char *)vec->data + i * vec->data_size, vec->data_size);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_vector_set(lion_app_t *app, lion_vector_t *vec,
                              const size_t i, const void *src) {
  if (vec->data == NULL || i >= vec->len || src == NULL) {
    return LION_STATUS_FAILURE;
  }
  memcpy((char *)vec->data + i * vec->data_size, src, vec->data_size);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_vector_resize(lion_app_t *app, lion_vector_t *vec,
                                 const size_t new_capacity) {
  void *data = lionu_realloc(app, vec->data, new_capacity * vec->data_size);
  if (data == NULL) {
    return LION_STATUS_FAILURE;
  }

  if (new_capacity < vec->len) {
    vec->len = new_capacity;
  }
  vec->capacity = new_capacity;
  vec->data = data;
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_vector_push(lion_app_t *app, lion_vector_t *vec,
                               const void *src) {
  if (src == NULL) {
    return LION_STATUS_FAILURE;
  }

  if (vec->len == vec->capacity) {
    // The vector is full so we have to reallocate
    // Default strategy is duplicate the current capacity
    // Perhaps the user can customize the behaviour?
    size_t new_size = 2 * vec->capacity * vec->data_size;
    if (new_size == 0) {
      new_size = vec->data_size;
    }
    logi_debug("Reallocating for %d B", new_size);
    void *new_data = lionu_realloc(app, vec->data, new_size);
    if (new_data == NULL) {
      return LION_STATUS_FAILURE;
    }
    memcpy((char *)new_data + vec->len * vec->data_size, src, vec->data_size);
    vec->data = new_data;
    vec->len++;
    vec->capacity = new_size / vec->data_size;
  } else {
    // It can be assumed that vec->len < vec->capacity, so this
    // means we dont have to allocate more memory.
    memcpy((char *)vec->data + vec->len * vec->data_size, src, vec->data_size);
    vec->len++;
  }
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_vector_extend_array(lion_app_t *app, lion_vector_t *vec,
                                       const void *src, const size_t len) {
  if (src == NULL) {
    return LION_STATUS_FAILURE;
  }

  // size_t delta = vec->len + len - vec->capacity;
  int64_t _delta = (int64_t)(vec->len + len - vec->capacity);
  size_t delta = 0;
  if (_delta > 0) {
    delta = (size_t)_delta;
  }
  if (delta > 0) {
    // The vector does not have enough space so we have to allocate more
    // memory
    logi_info("Allocating memory for extension of vector");
    logi_debug("Allocating %d more bytes", delta * vec->data_size);
    void *new_data =
        lionu_realloc(app, vec->data, (vec->capacity + delta) * vec->data_size);
    if (new_data == NULL) {
      logi_error("Reallocation failed");
      return LION_STATUS_FAILURE;
    }
    logi_debug("Copying new memory");
    memcpy((char *)new_data + vec->len * vec->data_size, src,
           len * vec->data_size);
    vec->data = new_data;
    vec->len += len;
    vec->capacity += delta;
    logi_debug("New len is %d", vec->len);
    logi_debug("New capacity is %d", vec->capacity);
  } else {
    // It can be assumed that vec->len < vec->capacity, so this
    // means we dont have to allocate more memory.
    memcpy((char *)vec->data + vec->len * vec->data_size, src,
           len * vec->data_size);
    vec->len += len;
  }
  return LION_STATUS_SUCCESS;
}

extern inline size_t lion_vector_total_size(lion_app_t *app,
                                            const lion_vector_t *vec) {
  return vec->len * vec->data_size;
}

extern inline size_t lion_vector_alloc_size(lion_app_t *app,
                                            const lion_vector_t *vec) {
  return vec->capacity * vec->data_size;
}
