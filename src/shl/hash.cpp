
#include "shl/hash.hpp"

hash_t hash_data(const void *data, u64 size)
{
    return hash_data(data, size, DEFAULT_MURMUR2_SEED);
}

hash_t hash_data(const void *_data, u64 size, u64 seed)
{
    const u64 m = 0xc6a4a7935bd1e995ull;
    const int r = 47;

    u64 h = seed ^ (size * m);

    const u64 *data = (const u64 *)_data;
    const u64 *end = data + (size / sizeof(u64));

    while(data != end)
    {
        u64 k = *data++;

        k *= m; 
        k ^= k >> r; 
        k *= m; 

        h ^= k;
        h *= m; 
    }

    const unsigned char *data2 = (const unsigned char *)data;

    switch(size & 7)
    {
        case 7: h ^= static_cast<u64>(data2[6]) << 48;
        case 6: h ^= static_cast<u64>(data2[5]) << 40;
        case 5: h ^= static_cast<u64>(data2[4]) << 32;
        case 4: h ^= static_cast<u64>(data2[3]) << 24;
        case 3: h ^= static_cast<u64>(data2[2]) << 16;
        case 2: h ^= static_cast<u64>(data2[1]) << 8;
        case 1: h ^= static_cast<u64>(data2[0]);
                h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
} 
