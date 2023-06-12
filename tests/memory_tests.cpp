
#include <t1/t1.hpp>

#include "shl/memory.hpp"

define_test(zeroed_alloc_allocates_zeroed_memory)
{
    int *x = allocate_memory<int, true>();

    assert_not_equal(x, nullptr);
    assert_equal(*x, 0);

    free_memory(x);
}

define_test(fill_memory_sets_memory)
{
    int *x = allocate_memory<int>();

    fill_memory(x, 1, 0x00);

    assert_equal(*x, 0);

    free_memory(x);
}

define_test(fill_memory_sets_memory2)
{
    int *x = allocate_memory<int>();

    fill_memory((void*)x, sizeof(int), 0x00);

    assert_equal(*x, 0);

    free_memory(x);
}

define_test(fill_memory_sets_memory3)
{
    int *x = allocate_memory<int>();

    fill_memory(x, 0x00);

    assert_equal(*x, 0);

    free_memory(x);
}

define_default_test_main()
