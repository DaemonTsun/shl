
#include "t1/t1.hpp"
#include "shl/allocator_arena.hpp"

define_test(arena_init_initializes_arena)
{
    arena _a;

    init(&_a, 128);

    {
        arena a = _a;

        assert_equal(arena_remaining_size(a), 128);
        assert_not_equal(a.start, nullptr);
        assert_not_equal(a.end, nullptr);
        assert_not_equal(a.start, a.end);
    }

    free(&_a);
}

define_test(alloc_uses_arena_allocator)
{
    arena _a;

    init(&_a, 128);

    {
        arena a = _a;

        assert_equal(arena_remaining_size(a), 128);

        allocator alloc = arena_allocator(&a);

        s64 *x = AllocT(alloc, s64);

        assert_equal(arena_remaining_size(a), 120);
        assert_not_equal(x, nullptr);
    }

    free(&_a);
}

define_test(realloc_reuses_last_alloc)
{
    arena _a;

    init(&_a, 128);

    {
        arena a = _a;

        assert_equal(arena_remaining_size(a), 128);

        allocator alloc = arena_allocator(&a);

        s64 *x = AllocT(alloc, s64);

        assert_equal(arena_remaining_size(a), 120);

        // since x is the last entry, its simply expanded
        void *ptr = Realloc(alloc, x, sizeof(s64), 32);

        assert_equal(arena_remaining_size(a), 96);
        assert_equal((void*)x, ptr);

        s64 *y = AllocT(alloc, s64);
        assert_equal(arena_remaining_size(a), 88);
        
        // ptr is not at end, cannot simply expand, must be reallocated
        ptr = Realloc(alloc, ptr, sizeof(s64), 64);
        assert_equal(arena_remaining_size(a), 24);
        assert_not_equal((void*)x, ptr);
        assert_not_equal((void*)y, ptr);
    }

    free(&_a);
}

define_test(free_only_frees_last_alloc)
{
    arena _a;

    init(&_a, 128);

    {
        arena a = _a;

        assert_equal(arena_remaining_size(a), 128);

        allocator alloc = arena_allocator(&a);

        s64 *x = AllocT(alloc, s64);

        assert_equal(arena_remaining_size(a), 120);
        assert_not_equal(x, nullptr);

        x = FreeT(alloc, x, s64);

        // x was last, is freed, and remaining size is adjusted.
        assert_equal(arena_remaining_size(a), 128);
        assert_equal(x, nullptr);

        x = AllocT(alloc, s64);
        s64 *y = AllocT(alloc, s64);

        assert_equal(arena_remaining_size(a), 112);
        assert_not_equal(x, nullptr);
        assert_not_equal(y, nullptr);

        x = FreeT(alloc, x, s64);

        // x is "freed", but remaining size remained the same because
        // x was not the last entry.
        assert_equal(arena_remaining_size(a), 112);
        assert_equal(x, nullptr);
        assert_not_equal(y, nullptr);

        y = FreeT(alloc, y, s64);

        // y was the last entry and was freed, but size is not 128 because
        // we don't track anything inside the arena except the last entry.
        assert_equal(arena_remaining_size(a), 120);
        assert_equal(y, nullptr);
    }

    free(&_a);
}

define_default_test_main()
