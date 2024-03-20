
#include <t1/t1.hpp>

#include "shl/memory.hpp"

define_test(alloc_allocates_memory)
{
    int *x = (int*)allocate_memory(sizeof(int));

    assert_not_equal(x, nullptr);

    free_memory(x);
}

define_test(zeroed_alloc_allocates_zeroed_memory)
{
    int *x = (int*)allocate_zeroed_memory(sizeof(int));

    assert_not_equal(x, nullptr);
    assert_equal(*x, 0);

    free_memory(x);
}

define_test(fill_memory_sets_memory)
{
    int *x = allocate_memory<int>();

    // when explicitly typed, last parameter is count of items
    fill_memory(x, 0x00, 1);

    assert_equal(*x, 0);

    free_memory(x);
}

define_test(fill_memory_sets_memory2)
{
    int *x = allocate_memory<int>();

    // when void*, last parameter is byte count
    fill_memory((void*)x, 0x00, sizeof(int));

    assert_equal(*x, 0);

    free_memory(x);
}

define_test(fill_memory_sets_memory3)
{
    int *x = allocate_memory<int>();

    // fills entire memory pointed to, with size of pointed-to type
    fill_memory(x, 0x00);

    assert_equal(*x, 0);

    free_memory(x);
}

define_test(default_allocator_allocates_and_deallocates_memory)
{
    allocator a = default_allocator;
    
    int *x = (int*)a.alloc(a.context, nullptr, 0, sizeof(int));

    assert_not_equal(x, nullptr);
    *x = 10;
    assert_equal(*x, 10);

    x = (int*)a.alloc(a.context, x, sizeof(int), 0);

    assert_equal(x, nullptr);
}

// Alloc and Free macro
define_test(default_allocator_allocates_and_deallocates_memory2)
{
    allocator a = default_allocator;
    
    int *x = (int*)Alloc(a, sizeof(int));

    assert_not_equal(x, nullptr);
    *x = 10;
    assert_equal(*x, 10);

    x = (int*)Free(a, x, sizeof(int));

    assert_equal(x, nullptr);
}

// AllocT and FreeT macros
define_test(default_allocator_allocates_and_deallocates_memory3)
{
    allocator a = default_allocator;
    
    int *x = AllocT(a, int);

    assert_not_equal(x, nullptr);
    *x = 10;
    assert_equal(*x, 10);

    x = FreeT(a, x, int);

    assert_equal(x, nullptr);
}

define_default_test_main()
