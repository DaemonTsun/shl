
#include <t1/t1.hpp>

#include "shl/allocator.hpp"

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
