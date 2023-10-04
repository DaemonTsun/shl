#pragma once

// number_types.hpp
// v1.1
// max / min values
// v1.0
// defines common number types

#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;


template<typename T> struct _max_value { };
#define define_max_value(T, Val) template<> struct _max_value<T>     { static constexpr T value = Val; };
#define max_value(T) _max_value<T>::value

template<typename T> struct _min_value { };
#define define_min_value(T, Val) template<> struct _min_value<T>     { static constexpr T value = Val; };
#define min_value(T) _min_value<T>::value

define_max_value(u8,  UINT8_MAX);
define_max_value(u16, UINT16_MAX);
define_max_value(u32, UINT32_MAX);
define_max_value(u64, UINT64_MAX);
define_max_value(s8,  INT8_MAX);
define_max_value(s16, INT16_MAX);
define_max_value(s32, INT32_MAX);
define_max_value(s64, INT64_MAX);

define_min_value(u8,  0);
define_min_value(u16, 0);
define_min_value(u32, 0);
define_min_value(u64, 0);
define_min_value(s8,  INT8_MIN);
define_min_value(s16, INT16_MIN);
define_min_value(s32, INT32_MIN);
define_min_value(s64, INT64_MIN);

