
#include <stdlib.h>

#include "shl/sort.hpp"

void sort(void *ptr, u64 count, u64 size, compare_function_p<void> comp)
{
    // TODO: implement this
    qsort(ptr, count, size, comp);
}

void *sorted_search(const void *key, void *ptr, u64 count, u64 size, compare_function_p<void> comp)
{
    // TODO: implement this
    return bsearch(key, ptr, count, size, comp);
}
