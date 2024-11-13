
#pragma once

#include "shl/array.hpp"

template<typename T>
struct array2d
{
    s32 width;
    s32 height;
    array<T> entries;

    T *operator[](s64 row) { return entries.data + row * this->width; }
};

template<typename T>
bool operator==(const array2d<T> &lhs, const array2d<T> &rhs)
{
    if ((lhs.width != rhs.width) || (lhs.height != rhs.height))
        return false;

    return lhs.entries == rhs.entries;
}

template<typename T>
void init(array2d<T> *arr)
{
    assert(arr != nullptr);

    init(&arr->entries);
    arr->width = 0;
    arr->height = 0;
}

template<typename T>
void init(array2d<T> *arr, s32 width, s32 height)
{
    assert(arr != nullptr);
    assert(width  >= 0);
    assert(height >= 0);

    s64 count = width * height;
    init(&arr->entries, count);
    arr->width = width;
    arr->height = height;
}

template<typename T>
void free_values(array2d<T> *arr)
{
    assert(arr != nullptr);
    
    free_values(&arr->entries);
}

template<bool FreeValues = false, typename T>
void free(array2d<T> *arr)
{
    assert(arr != nullptr);

    if constexpr (FreeValues) free_values(arr);

    free(&arr->entries);
}

template<bool FreeValues = false, typename T>
bool resize(array2d<T> *arr, s32 width, s32 height)
{
    assert(arr != nullptr);
    assert(width  >= 0);
    assert(height >= 0);

    if ((arr->width == width) && (arr->height == height))
        return true;

    array<T> *entries = &arr->entries;

    if ((width == 0) || (height == 0))
    {
        resize<FreeValues, T>(entries, 0);
        arr->width = width;
        arr->height = height;
        return true;
    }

    s64 count = width * height;
    s32 oldwidth = arr->width;
    s32 oldheight = arr->height;

    if (!reserve(entries, count))
        return false;

    if (oldwidth < width && oldwidth > 0)
    {
        // per row, need to move entries to match old layout
        // and fill new spots in each row with 0

        // backwards because we're moving things, can't overwrite them
        // when going backwards.
        for (s32 row = Min(height, oldheight) - 1; row >= 0; --row)
        {
            s64 stride = oldwidth * sizeof(T);
            T *move_from = entries->data + row * oldwidth;
            T *move_to   = entries->data + row * width;
            T *row_leftovers = move_to + oldwidth;

            if (move_from != move_to)
                move_memory((void*)move_from, (void*)move_to, stride);

            fill_memory((void*)row_leftovers, 0, (width - oldwidth) * sizeof(T));
        }
    }
    else if (oldwidth > width)
    {
        for (s32 row = 0; row < Min(height, oldheight); ++row)
        {
            s64 stride = width * sizeof(T);
            T *move_from = entries->data + row * oldwidth;
            T *move_to   = entries->data + row * width;

            if constexpr (FreeValues)
            {
                T *row_leftovers = move_from + width;
                
                for (s32 c = 0; c < (oldwidth - width); ++c)
                    free(row_leftovers + c);
            }

            if (move_from != move_to)
                move_memory((void*)move_from, (void*)move_to, stride);
        }
    }

    if (oldheight < height)
    {
        s64 offset = oldheight * width;
        s64 rest_size = (height - oldheight) * width * sizeof(T);
        fill_memory((void*)(entries->data + offset), 0, rest_size);
    }
    else if (oldheight > height)
    {
        if constexpr (FreeValues)
        {
            s64 offset = height * width;
            s64 count = oldheight * oldwidth - offset;

            for (s64 i = 0; i < count; ++i)
                free(entries->data + offset + i);
        }
    }

    arr->width = width;
    arr->height = height;
    entries->size = count;

    return true;
}

#define for_array2d_XYC(X_Var, Y_Var, C_Var, ARRAY)\
    if constexpr (auto *C_Var = (ARRAY)->entries.data; true)\
    for (s64 Y_Var = 0; Y_Var < (ARRAY)->height; ++Y_Var)\
    for (s64 X_Var = 0; X_Var < (ARRAY)->width;  ++X_Var, ++C_Var)

#define for_array2d_YC(Y_Var, C_Var, ARRAY)\
    for_array2d_XYC(C_Var##_x, Y_Var, C_Var, ARRAY)

#define for_array2d_C(C_Var, ARRAY)\
    for_array2d_XYC(C_Var##_x, C_Var##_y, C_Var, ARRAY)

#define for_array2d(...) GET_MACRO3(__VA_ARGS__, for_array2d_XYC, for_array2d_YC, for_array2d_C)(__VA_ARGS__)

