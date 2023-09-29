
#pragma once

#include "shl/macros.hpp"

template<typename T, u64 N>
T *array_data(T (*arr)[N])
{
    return *arr;
}

template<typename T, u64 N>
u64 array_size(T (*arr)[N])
{
    return N;
}

#define for_array_IV(I_Var, V_Var, ARRAY)\
    auto *V_Var = array_data(ARRAY);\
    for (u64 I_Var = 0; I_Var < array_size(ARRAY); ++I_Var, ++V_Var)

#define for_array_V(V_Var, ARRAY)\
    for_array_IV(V_Var##_index, V_Var, ARRAY)

#define for_array(...) GET_MACRO2(__VA_ARGS__, for_array_IV, for_array_V)(__VA_ARGS__)

