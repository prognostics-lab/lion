#include <gsl/gsl_math.h>
#include <stddef.h>
#include <stdint.h>

#include <lionu/math.h>

// clang-format off
#define __LION_GENERATOR_ALLTYPES(macro)                                       \
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

#define _SUM_GENERATOR(T)                                                      \
  T lion_sum_##T(T *vals, size_t count) {                                      \
    T res = 0;                                                                 \
    for (size_t i = 0; i < count; i++) {                                       \
      res += vals[i];                                                          \
    }                                                                          \
    return res;                                                                \
  }

#define _POLYVAL_GENERATOR(T)                                                  \
  T lion_polyval_##T(T x, T *coeffs, u32 count) {                              \
    T res = 0;                                                                 \
    for (u32 i = 0; i < count; i++) {                                          \
      res += coeffs[i] * gsl_pow_uint(x, i);                                   \
    }                                                                          \
    return res;                                                                \
  }

// clang-format off
__LION_GENERATOR_ALLTYPES(_SUM_GENERATOR)
__LION_GENERATOR_ALLTYPES(_POLYVAL_GENERATOR)
// clang-format on
