/// @file
#pragma once

#include "status.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lion_app lion_app_t;

/// @addtogroup types
/// @{

/// Variable length vector of data.
typedef struct lion_vector {
  void  *data;      ///< Data container in heap.
  size_t data_size; ///< Size of each element.
  size_t len;       ///< Length of the vector.
  size_t capacity;  ///< Capacity of the vector.
} lion_vector_t;

/// @}

/// @addtogroup functions
/// @{

/* Vector initialization */

/// Create new empty vector.
///
/// @param[in]  app        Simulation context, can be NULL.
/// @param[in]  data_size  Size of each element.
/// @param[out] out        New vector.
lion_status_t lion_vector_new(lion_app_t *app, const size_t data_size, lion_vector_t *out);

/// Create new vector filled with zeros.
///
/// @param[in]  app        Simulation context, can be NULL.
/// @param[in]  len        Number of elements.
/// @param[in]  data_size  Size of each element.
/// @param[out] out        New vector.
lion_status_t lion_vector_zero(lion_app_t *app, const size_t len, const size_t data_size, lion_vector_t *out);

/// Create empty vector with preallocated size.
///
/// @param[in]  app        Simulation context, can be NULL.
/// @param[in]  capacity   Preallocated size.
/// @param[in]  data_size  Size of each element.
/// @param[out] out        New vector.
lion_status_t lion_vector_with_capacity(lion_app_t *app, const size_t capacity, const size_t data_size, lion_vector_t *out);

/// Create vector from array.
///
/// @param[in]  app        Simulation context, can be NULL.
/// @param[in]  data       Elements of the array.
/// @param[in]  len        Number of elements.
/// @param[in]  data_size  Size of each element.
/// @param[out] out        New vector.
lion_status_t lion_vector_from_array(lion_app_t *app, const void *data, const size_t len, const size_t data_size, lion_vector_t *out);

/// Create vector from a CSV file.
///
/// @param[in]  app        Simulation context, can be NULL.
/// @param[in]  filename   Name of the file.
/// @param[in]  data_size  Size of each element.
/// @param[in]  format     Format of each line.
/// @param[out] out        New vector.
lion_status_t lion_vector_from_csv(lion_app_t *app, const char *filename, const size_t data_size, const char *format, lion_vector_t *out);

/// Create vector of evenly spaced doubles.
///
/// @param[in]  app        Simulation context, can be NULL.
/// @param[in]  low        Lower limit.
/// @param[in]  high       Upper limit.
/// @param[in]  num        Number of elements.
/// @param[out] out        New vector.
lion_status_t lion_vector_linspace_d(lion_app_t *app, double low, double high, int num, lion_vector_t *out);

/// Create vector of evenly spaced floats.
///
/// @param[in]  app        Simulation context, can be NULL.
/// @param[in]  low        Lower limit.
/// @param[in]  high       Upper limit.
/// @param[in]  num        Number of elements.
/// @param[out] out        New vector.
lion_status_t lion_vector_linspace_f(lion_app_t *app, float low, float high, int num, lion_vector_t *out);

/* Vector saving */

/// Save a vector into a CSV file.
///
/// @param[in]  app        Simulation context, can be NULL.
/// @param[in]  vec        Vector to save.
/// @param[in]  header     Header to use for the CSV file. For no header pass NULL.
/// @param[in]  filename   Name of the file.
lion_status_t lion_vector_to_csv(lion_app_t *app, lion_vector_t *vec, const char *header, const char *filename);

/* Vector finalization */

/// Destroy a vector.
lion_status_t lion_vector_cleanup(lion_app_t *app, const lion_vector_t *const vec);

/* Vector attributes */

/// Get a given element from the vector.
///
/// @param[in]  app         Simulation context, can be NULL.
/// @param[in]  vec         Vector.
/// @param[in]  i           Index of the element.
/// @param[out] out         Pointer to the location on which to store the output.
lion_status_t lion_vector_get(lion_app_t *app, const lion_vector_t *vec, const size_t i, void *out);
int8_t        lion_vector_get_i8(lion_app_t *app, const lion_vector_t *vec, const size_t i);
int16_t       lion_vector_get_i16(lion_app_t *app, const lion_vector_t *vec, const size_t i);
int32_t       lion_vector_get_i32(lion_app_t *app, const lion_vector_t *vec, const size_t i);
int64_t       lion_vector_get_i64(lion_app_t *app, const lion_vector_t *vec, const size_t i);
uint8_t       lion_vector_get_u8(lion_app_t *app, const lion_vector_t *vec, const size_t i);
uint16_t      lion_vector_get_u16(lion_app_t *app, const lion_vector_t *vec, const size_t i);
uint32_t      lion_vector_get_u32(lion_app_t *app, const lion_vector_t *vec, const size_t i);
uint64_t      lion_vector_get_u64(lion_app_t *app, const lion_vector_t *vec, const size_t i);
float         lion_vector_get_f(lion_app_t *app, const lion_vector_t *vec, const size_t i);
double        lion_vector_get_d(lion_app_t *app, const lion_vector_t *vec, const size_t i);
void         *lion_vector_get_p(lion_app_t *app, const lion_vector_t *vec, const size_t i);

/// Set a given element from the vector. Copies the original element.
///
/// @param[in]  app         Simulation context, can be NULL.
/// @param[in]  vec         Vector.
/// @param[in]  i           Index of the element.
/// @param[in] out          Pointer to the location to copy into the vector.
lion_status_t lion_vector_set(lion_app_t *app, lion_vector_t *vec, const size_t i, const void *src);

// TODO: See what happens when `new_capacity` < `old_capacity`

/// Resizes the vector.
///
/// @param[in]  app           Simulation context, can be NULL.
/// @param[in]  vec           Vector to resize.
/// @param[in]  new_capacity  New capacity for the vector.
lion_status_t lion_vector_resize(lion_app_t *app, lion_vector_t *vec, const size_t new_capacity);

/// Pushes an element into the vector.
///
/// @param[in]  app           Simulation context, can be NULL.
/// @param[in]  vec           Vector to push into.
/// @param[in]  src           Element to push. Clones this element.
lion_status_t lion_vector_push(lion_app_t *app, lion_vector_t *vec, const void *src);
lion_status_t lion_vector_push_d(lion_app_t *app, lion_vector_t *vec, double src);
lion_status_t lion_vector_push_f(lion_app_t *app, lion_vector_t *vec, float src);

/// Extends the vector using an array.
///
/// @param[in]  app           Simulation context, can be NULL.
/// @param[in]  vec           Vector to resize.
/// @param[in]  src           Elements to push. Get cloned.
/// @param[in]  len           Number of elements to push.
lion_status_t lion_vector_extend_array(lion_app_t *app, lion_vector_t *vec, const void *src, const size_t len);

/// Total size of the vector.
///
/// @param[in]  app           Simulation context, can be NULL.
/// @param[in]  vec           Vector to fetch.
size_t lion_vector_total_size(lion_app_t *app, const lion_vector_t *vec);

/// Allocated size of the vector.
///
/// @param[in]  app           Simulation context, can be NULL.
/// @param[in]  vec           Vector to fetch.
size_t lion_vector_alloc_size(lion_app_t *app, const lion_vector_t *vec);

/// @}

#ifdef __cplusplus
}
#endif
