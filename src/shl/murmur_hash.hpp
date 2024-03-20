
#pragma once

#include "shl/number_types.hpp"
#include "shl/bits.hpp"
#include "shl/endian.hpp"

#define DEFAULT_MURMUR3_SEED 0xc70f6907

// https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp
//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
//
// Note - The x86 and x64 versions do _not_ produce the same results, as the
// algorithms are optimized for their respective platforms. You can still
// compile and run any of them on any platform, but your performance with the
// non-native version will be less than optimal.
// 
//-----------------------------------------------------------------------------
//
// This is modified to be constexpr. 

#if defined(_MSC_VER)

#define FORCE_INLINE	__forceinline
#define BIG_CONSTANT(x) (x)

#else

#define	FORCE_INLINE inline __attribute__((always_inline))
#define BIG_CONSTANT(x) (x##LLU)

#endif

constexpr FORCE_INLINE u32 getblock32(const char *p, int i)
{
    // might need to check endianness
    u32 block = static_cast<u8>(p[i])     << 0  | 
		        static_cast<u8>(p[i + 1]) << 8  | 
		        static_cast<u8>(p[i + 2]) << 16 | 
		        static_cast<u8>(p[i + 3]) << 24;

	return block;
}

// Finalization mix - force all bits of a hash block to avalanche
constexpr FORCE_INLINE u32 fmix32(u32 h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}

// why the hell is len an int
constexpr u32 MurmurHash3_x86_32(const char *data, int len, u32 seed)
{
    const int nblocks = len / 4;

    u32 h1 = seed;

    const u32 c1 = 0xcc9e2d51;
    const u32 c2 = 0x1b873593;

    // body
    const char *blocks = data + nblocks * 4;

    for(int i = -nblocks; i; i++)
    {
        u32 k1 = getblock32(blocks, i * 4);

        k1 *= c1;
        k1 = rotl(k1, 15);
        k1 *= c2;
        
        h1 ^= k1;
        h1 = rotl(h1, 13); 
        h1 = h1 * 5 + 0xe6546b64;
    }

    // tail
    const char *tail = data + nblocks * 4;

    u32 k1 = 0;

    switch (len & 3)
    {
    case 3: k1 ^= tail[2] << 16; [[fallthrough]];
    case 2: k1 ^= tail[1] << 8;  [[fallthrough]];
    case 1: k1 ^= tail[0];
            k1 *= c1; k1 = rotl(k1, 15);
            k1 *= c2;
            h1 ^= k1;
    };

    h1 ^= len;
    h1 = fmix32(h1);

    return h1;
} 

constexpr u32 MurmurHash3_x86_32(const char *data, int len)
{
    return MurmurHash3_x86_32(data, len, DEFAULT_MURMUR3_SEED);
}

constexpr u32 _const_string_len(const char *str)
{
    u32 ret = 0;

    while (*str)
    {
        ret++;
        str++;
    }

    return ret;
}

#define _STR2(x) #x
#define _STR(x) _STR2(x)

#define __FILE_HASH__ (MurmurHash3_x86_32(__FILE__, _const_string_len(__FILE__)))
#define __LINE_HASH__ (MurmurHash3_x86_32(__FILE__ _STR(__LINE__), _const_string_len(__FILE__ _STR(__LINE__))))
