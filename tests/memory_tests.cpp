
#include <t1/t1.hpp>

#include "shl/memory.hpp"

define_test(zeroed_alloc_allocates_zeroed_memory)
{
    int *x = allocate_memory<int, true>();

    assert_not_equal(x, nullptr);
    assert_equal(*x, 0);

    free_memory(x);
}

define_default_test_main()
