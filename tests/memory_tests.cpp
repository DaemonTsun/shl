
#include <t1/t1.hpp>

#include "shl/memory.hpp"

define_test(alloc_allocates_memory)
{
    int *x = (int*)alloc(sizeof(int));

    assert_not_equal(x, nullptr);

    dealloc(x, sizeof(int));
}

define_test(fill_memory_sets_memory)
{
    int *x = alloc<int>();

    // when void*, last parameter is byte count
    fill_memory((void*)x, 0x00, sizeof(int));

    assert_equal(*x, 0);

    dealloc_T(x);
}

define_test(fill_memory_sets_memory2)
{
    int *x = alloc<int>();

    // fills entire memory pointed to, with size of pointed-to type
    fill_memory(x, 0x00);

    assert_equal(*x, 0);

    dealloc_T(x);
}

define_default_test_main()
