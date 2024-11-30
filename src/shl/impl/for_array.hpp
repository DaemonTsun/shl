
#pragma once

#include "shl/macros.hpp"

template<typename T, s64 N>
T *array_data(T (*arr)[N])
{
    return *arr;
}

template<typename T, s64 N>
s64 array_size([[maybe_unused]] T (*arr)[N])
{
    return N;
}

#define for_array_IV(I_Var, V_Var, ARRAY)\
    if constexpr (auto *V_Var = array_data(ARRAY); true)\
    for (s64 I_Var = 0; I_Var < array_size(ARRAY); ++I_Var, ++V_Var)

#define for_array_V(V_Var, ARRAY)\
    for_array_IV(V_Var##_index, V_Var, ARRAY)

#define for_array(...) GET_MACRO2(__VA_ARGS__, for_array_IV, for_array_V)(__VA_ARGS__)


// reverse lol
#define for_array_reverse_IV(I_Var, V_Var, ARRAY)\
    if constexpr (s64 I_Var = array_size(ARRAY) - 1; true)\
    if constexpr (auto *V_Var = array_data(ARRAY) + I_Var; true)\
    for (; I_Var >= 0; --I_Var, --V_Var)

#define for_array_reverse_V(V_Var, ARRAY)\
    for_array_reverse_IV(V_Var##_index, V_Var, ARRAY)

#define for_array_reverse(...) GET_MACRO2(__VA_ARGS__, for_array_reverse_IV, for_array_reverse_V)(__VA_ARGS__)
