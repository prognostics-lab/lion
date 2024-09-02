#include <gsl/gsl_math.h>
#include <stddef.h>
#include <stdint.h>

#include <lionu/math.h>

// clang-format off
#define __lion_generator_alltypes(macro)                                       \
  macro(i8)                                                                    \
  macro(i16)                                                                   \
  macro(i32)                                                                   \
  macro(i64)                                                                   \
  macro(u8)                                                                    \
  macro(u16)                                                                   \
  macro(u32)                                                                   \
  macro(u64)                                                                   \
  macro(float)                                                                 \
  macro(double)
// clang-format on

#define _sum_generator(T)                                                      \
  T lion_sum_##T(T *vals, size_t count) {                                      \
    T res = 0;                                                                 \
    for (size_t i = 0; i < count; i++) {                                       \
      res += vals[i];                                                          \
    }                                                                          \
    return res;                                                                \
  }

#define _polyval_generator(T)                                                  \
  T lion_polyval_##T(T x, T *coeffs, u32 count) {                              \
    T res = 0;                                                                 \
    for (u32 i = 0; i < count; i++) {                                          \
      res += coeffs[i] * gsl_pow_uint(x, i);                                   \
    }                                                                          \
    return res;                                                                \
  }

// clang-format off
__lion_generator_alltypes(_sum_generator)
__lion_generator_alltypes(_polyval_generator)
// clang-format on
