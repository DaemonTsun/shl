
#include <stdlib.h>

#include "shl/sort.hpp"

void sort(void *ptr, u64 count, u64 size, compare_function<void> comp)
{
    // TODO: implement this
    qsort(ptr, count, size, comp);
}

void *search(const void *key, void *ptr, u64 count, u64 size, compare_function<void> comp)
{
    // TODO: implement this
    return bsearch(key, ptr, count, size, comp);
}
