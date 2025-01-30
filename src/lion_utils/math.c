#include <gsl/gsl_math.h>
#include <lionu/math.h>
#include <stddef.h>
#include <stdint.h>

#define __LION_GENERATOR_ALLTYPES(macro)                                                                                                             \
  macro(int8_t, i8);                                                                                                                                 \
  macro(int16_t, i16);                                                                                                                               \
  macro(int32_t, i32);                                                                                                                               \
  macro(int64_t, i64);                                                                                                                               \
  macro(uint8_t, u8);                                                                                                                                \
  macro(uint16_t, u16);                                                                                                                              \
  macro(uint32_t, u32);                                                                                                                              \
  macro(uint64_t, u64);                                                                                                                              \
  macro(float, f);                                                                                                                                   \
  macro(double, d);

#define _SUM_GENERATOR(T, S)                                                                                                                         \
  T lion_sum_##S(T *vals, size_t count) {                                                                                                            \
    T res = 0;                                                                                                                                       \
    for (size_t i = 0; i < count; i++) {                                                                                                             \
      res += vals[i];                                                                                                                                \
    }                                                                                                                                                \
    return res;                                                                                                                                      \
  }

#define _POLYVAL_GENERATOR(T, S)                                                                                                                     \
  T lion_polyval_##S(T x, T *coeffs, u32 count) {                                                                                                    \
    T res = 0;                                                                                                                                       \
    for (u32 i = 0; i < count; i++) {                                                                                                                \
      res += coeffs[i] * gsl_pow_uint(x, i);                                                                                                         \
    }                                                                                                                                                \
    return res;                                                                                                                                      \
  }

__LION_GENERATOR_ALLTYPES(_SUM_GENERATOR);
__LION_GENERATOR_ALLTYPES(_POLYVAL_GENERATOR);
