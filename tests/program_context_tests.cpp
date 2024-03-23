
#include "shl/program_context.hpp"
#include "shl/allocator_arena.hpp"
#include "shl/memory.hpp"

#include "t1/t1.hpp"

define_test(with_context_changes_context_for_block)
{
    arena _a;
    init(&_a, 128);
    defer { free(&_a); };

    arena a = _a;

    // uses current global allocator
    s64 *x = alloc<s64>();

    // obviously, arena 'a' was not used yet.
    assert_equal(arena_remaining_size(a), 128);

    // copy of the current context
    program_context nctx = *get_context_pointer();
    nctx.allocator = arena_allocator(&a);

    with_context(&nctx)
    {
        // since nctx uses arena 'a' as allocator, this allocation uses 'a'.
        s64 *y = alloc<s64>();

        assert_not_equal(y, nullptr);
        assert_equal(arena_remaining_size(a), 120);

        // y does not need to be freed here (since the arena will be freed all at once)

        // also it should be obvious but don't deallocate x inside here, 'a' has
        // no idea that x is not inside it.
    }

    // x should be deallocated since it used the generic global allocator
    dealloc(x);
}

define_test(with_allocator_changes_allocator_for_block)
{
    arena _a;
    init(&_a, 128);
    defer { free(&_a); };

    arena a = _a;

    // uses current global allocator
    s64 *x = alloc<s64>();

    // obviously, arena 'a' was not used yet.
    assert_equal(arena_remaining_size(a), 128);

    // also copies the current context
    with_allocator(arena_allocator(&a))
    {
        s64 *y = alloc<s64>();

        assert_not_equal(y, nullptr);
        assert_equal(arena_remaining_size(a), 120);

        with_allocator(default_allocator)
        {
            s64 *z = alloc<s64>();

            assert_not_equal(z, nullptr);

            // yep, not using arena 'a' here
            assert_equal(arena_remaining_size(a), 120);

            dealloc(z);
        }
    }

    dealloc(x);
}

define_default_test_main();
