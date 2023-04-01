
#pragma once

#define _for_array_vars(I_Var, V_Var, ARRAY)\
    u64 I_Var = 0;\
    auto *V_Var = (ARRAY)->data;

#define for_array_V(V_Var, ARRAY)\
    _for_array_vars(V_Var##_index, V_Var, ARRAY)\
    for (; V_Var##_index < array_size(ARRAY); ++V_Var##_index, ++V_Var)

#define for_array_IV(I_Var, V_Var, ARRAY)\
    _for_array_vars(I_Var, V_Var, ARRAY)\
    for (; I_Var < array_size(ARRAY); ++I_Var, ++V_Var)

#define for_array(...) GET_MACRO2(__VA_ARGS__, for_array_IV, for_array_V)(__VA_ARGS__)
