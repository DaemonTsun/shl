
#include "shl/murmur_hash.hpp"
#include "shl/hash.hpp"

hash_t hash_data(const void *data, u64 size)
{
    return MurmurHash3_x86_32(reinterpret_cast<const char*>(data), static_cast<u32>(size) /*WHY*/);
}

hash_t hash_data(const void *data, u64 size, u32 seed)
{
    return MurmurHash3_x86_32(reinterpret_cast<const char*>(data), static_cast<u32>(size) /*WHY*/, seed);
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

hash_t operator""_h(const char    *str, u64 size)
{
    return hash_data(str, size);
}

hash_t operator""_h(const wchar_t *str, u64 size)
{
    return hash_data(str, size);
}
