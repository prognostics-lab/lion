#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

i8     lion_clip_i8(i8 x, i8 low, i8 high);
i16    lion_clip_i16(i16 x, i16 low, i16 high);
i32    lion_clip_i32(i32 x, i32 low, i32 high);
i64    lion_clip_i64(i64 x, i64 low, i64 high);
u8     lion_clip_u8(u8 x, u8 low, u8 high);
u16    lion_clip_u16(u16 x, u16 low, u16 high);
u32    lion_clip_u32(u32 x, u32 low, u32 high);
u64    lion_clip_u64(u64 x, u64 low, u64 high);
float  lion_clip_f(float x, float low, float high);
double lion_clip_d(double x, double low, double high);

i8     lion_sum_i8(i8 *vals, size_t count);
i16    lion_sum_i16(i16 *vals, size_t count);
i32    lion_sum_i32(i32 *vals, size_t count);
i64    lion_sum_i64(i64 *vals, size_t count);
u8     lion_sum_u8(u8 *vals, size_t count);
u16    lion_sum_u16(u16 *vals, size_t count);
u32    lion_sum_u32(u32 *vals, size_t count);
u64    lion_sum_u64(u64 *vals, size_t count);
float  lion_sum_f(float *vals, size_t count);
double lion_sum_d(double *vals, size_t count);

i8     lion_polyval_i8(i8 x, i8 *coeffs, u32 count);
i16    lion_polyval_i16(i16 x, i16 *coeffs, u32 count);
i32    lion_polyval_i32(i32 x, i32 *coeffs, u32 count);
i64    lion_polyval_i64(i64 x, i64 *coeffs, u32 count);
u8     lion_polyval_u8(u8 x, u8 *coeffs, u32 count);
u16    lion_polyval_u16(u16 x, u16 *coeffs, u32 count);
u32    lion_polyval_u32(u32 x, u32 *coeffs, u32 count);
u64    lion_polyval_u64(u64 x, u64 *coeffs, u32 count);
float  lion_polyval_f(float x, float *coeffs, u32 count);
double lion_polyval_d(double x, double *coeffs, u32 count);

#ifdef __cplusplus
}
#endif
