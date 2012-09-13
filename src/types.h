#ifndef TYPES_H_
#define TYPES_H_
#include <cinttypes>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int_fast8_t fs8;
typedef int_fast16_t fs16;
typedef int_fast32_t fs32;
typedef int_fast64_t fs64;
typedef uint_fast8_t fu8;
typedef uint_fast16_t fu16;
typedef uint_fast32_t fu32;
typedef uint_fast64_t fu64;

#define maxs8 INT8_MAX
#define maxs16 INT16_MAX
#define maxs32 INT32_MAX
#define maxs64 INT64_MAX
#define maxu8 UINT8_MAX
#define maxu16 UINT16_MAX
#define maxu32 UINT32_MAX
#define maxu64 UINT64_MAX
#define maxfs8 INT_FAST8_MAX
#define maxfs16 INT_FAST16_MAX
#define maxfs32 INT_FAST32_MAX
#define maxfs64 INT_FAST64_MAX
#define maxfu8 UINT_FAST8_MAX
#define maxfu16 UINT_FAST16_MAX
#define maxfu32 UINT_FAST32_MAX
#define maxfu64 UINT_FAST64_MAX

//intptr_t
//intmax_t

#if 0
#if defined __GNUG__ && !(defined __clang__)
#define GCC
#warning Using GCC, swizzles will be slow, you should try clang!
    #endif
#if defined __clang__
#define CLANG
    #endif
#else
#define GCC
#endif

typedef float v16sf __attribute__ ((vector_size (sizeof(float)*16)));
typedef float v4sf __attribute__ ((vector_size (sizeof(float)*4)));
typedef int v4si __attribute__ ((vector_size (sizeof(int)*4)));
typedef union matrix_data
{
    v16sf full_matrix;
    v4sf matrix_vectors[4];
    float matrix_floats[16];
} matrix_data;

typedef union vector_data
{
    v4sf full_vector;
    float vector_floats[4];
} vector_data;

static const double PI = 3.14159265358979323846;
#define FLOAT_TOLERANCE 0.00001f
#define STOPPING_SPEED_TOLERANCE 1.0f
#define INERTIA 0.1f
#define TARGET_RADIUS 1.0f
#endif
