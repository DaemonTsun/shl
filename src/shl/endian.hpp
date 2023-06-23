
#pragma once

#include "shl/number_types.hpp"

#define IS_LITTLE_ENDIAN  ('ABCD'==0x41424344ul) 
#define IS_BIG_ENDIAN     ('ABCD'==0x44434241ul)

constexpr u32 force_big_endian(u32 value)
{
    if constexpr (IS_BIG_ENDIAN)
        return value;
    else
    {
        u32 ret = 0;
        ret |= ((value & 0xff) << 24);
        ret |= (((value >> 8) & 0xff)  << 16);
        ret |= (((value >> 16) & 0xff) << 8);
        ret |= (((value >> 24) & 0xff) << 0);

        return ret;
    }
}

constexpr u32 force_little_endian(u32 value)
{
    if constexpr (IS_LITTLE_ENDIAN)
        return value;
    else
    {
        u32 ret = 0;
        ret |= ((value & 0xff) << 24);
        ret |= (((value >> 8) & 0xff)  << 16);
        ret |= (((value >> 16) & 0xff) << 8);
        ret |= (((value >> 24) & 0xff) << 0);

        return ret;
    }
}
