
#include <t1/t1.hpp>

#include "shl/allocator.hpp"

define_test(default_allocator_allocates_and_deallocates_memory)
{
    allocator a = default_allocator;
    
    int *x = (int*)a.alloc(a.data, nullptr, 0, sizeof(int));

    assert_not_equal(x, nullptr);
    *x = 10;
    assert_equal(*x, 10);

    x = (int*)a.alloc(a.data, x, sizeof(int), 0);

    assert_equal(x, nullptr);
}

// allocator_alloc and Free macro
define_test(default_allocator_allocates_and_deallocates_memory2)
{
    allocator a = default_allocator;
    
    int *x = (int*)allocator_alloc(a, sizeof(int));

    assert_not_equal(x, nullptr);
    *x = 10;
    assert_equal(*x, 10);

    x = (int*)allocator_dealloc(a, x, sizeof(int));

    assert_equal(x, nullptr);
}

// allocator_alloc_T and allocator_dealloc_T macros
define_test(default_allocator_allocates_and_deallocates_memory3)
{
    allocator a = default_allocator;
    
    int *x = allocator_alloc_T(a, int);

    assert_not_equal(x, nullptr);
    *x = 10;
    assert_equal(*x, 10);

    x = allocator_dealloc_T(a, x, int);

    assert_equal(x, nullptr);
}

define_default_test_main()
