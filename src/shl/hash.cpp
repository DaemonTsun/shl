
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

hash_t hash(const char *string)
{
    u64 size = 0;

    const char *c = string;
    while(*c != '\0')
    {
        size++;
        c++;
    }

    return hash_data(reinterpret_cast<const void*>(string), size);
}

hash_t hash(const wchar_t *string)
{
    u64 size = 0;

    const wchar_t *c = string;
    while(*c != '\0')
    {
        size++;
        c++;
    }

    return hash_data(reinterpret_cast<const void*>(string), size * sizeof(wchar_t));
}

hash_t hash(const bool *v)
{
    return hash_data(reinterpret_cast<const void*>(v), sizeof(bool));
}

hash_t hash(const u8  *v)
{
    return hash_data(reinterpret_cast<const void*>(v), sizeof(u8));
}

hash_t hash(const u16 *v)
{
    return hash_data(reinterpret_cast<const void*>(v), sizeof(u16));
}

hash_t hash(const u32 *v)
{
    return hash_data(reinterpret_cast<const void*>(v), sizeof(u32));
}

hash_t hash(const u64 *v)
{
    return hash_data(reinterpret_cast<const void*>(v), sizeof(u64));
}

hash_t hash(const s8  *v)
{
    return hash_data(reinterpret_cast<const void*>(v), sizeof(s8));
}

hash_t hash(const s16 *v)
{
    return hash_data(reinterpret_cast<const void*>(v), sizeof(s16));
}

hash_t hash(const s32 *v)
{
    return hash_data(reinterpret_cast<const void*>(v), sizeof(s32));
}

hash_t hash(const s64 *v)
{
    return hash_data(reinterpret_cast<const void*>(v), sizeof(s64));
}

hash_t hash(const float *v)
{
    return hash_data(reinterpret_cast<const void*>(v), sizeof(float));
}

hash_t hash(const double *v)
{
    return hash_data(reinterpret_cast<const void*>(v), sizeof(double));
}

hash_t hash(const long double *v)
{
    return hash_data(reinterpret_cast<const void*>(v), sizeof(long double));
}

hash_t hash(const void **v)
{
    return hash_data(reinterpret_cast<const void*>(v), sizeof(const void*));
}

