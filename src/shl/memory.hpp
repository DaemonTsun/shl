
#pragma once

#include "shl/number_types.hpp"

void *allocate_memory(u64 size);
void *allocate_zeroed_memory(u64 size);

template<typename T, bool Zero = false>
T *allocate_memory()
{
    if constexpr (Zero)
        return reinterpret_cast<T*>(allocate_zeroed_memory(sizeof(T)));
    else
        return reinterpret_cast<T*>(allocate_memory(sizeof(T)));
}

template<typename T, bool Zero = false>
T *allocate_memory(u64 n_elements)
{
    if constexpr (Zero)
        return reinterpret_cast<T*>(allocate_zeroed_memory(sizeof(T) * n_elements));
    else
        return reinterpret_cast<T*>(allocate_memory(sizeof(T) * n_elements));
}

void *reallocate_memory(void *ptr, u64 size);

template<typename T>
T *reallocate_memory(T *ptr, u64 n_elements)
{
    return reinterpret_cast<T*>(reallocate_memory(reinterpret_cast<void*>(ptr), sizeof(T) * n_elements));
}

void *move_memory(const void *from, void *to, u64 size);
void *copy_memory(const void *from, void *to, u64 size);
void free_memory(void *ptr);

template<typename T>
void free_memory(T *ptr)
{
    free_memory(reinterpret_cast<void*>(ptr));
}

void fill_memory(void *ptr, u64 size, u8 byte);

template<typename T>
void fill_memory(T *ptr, u8 byte)
{
    fill_memory(reinterpret_cast<void*>(ptr), sizeof(T), byte);
}

template<typename T>
void fill_memory(T *ptr, u64 count, u8 byte)
{
    fill_memory(reinterpret_cast<void*>(ptr), count * sizeof(T), byte);
}

