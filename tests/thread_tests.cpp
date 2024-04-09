
#include "t1/t1.hpp"
#include "shl/thread.hpp"
#include "shl/time.hpp"

void *empty_thread_func(void*)
{
    return nullptr;
}

define_test(thread_create_creates_thread)
{
    thread t{};
    error err{};

    bool ok = thread_create(&t, empty_thread_func, nullptr, nullptr, &err);

    assert_equal(ok, true);
    assert_equal(err.error_code, 0);
    assert_equal(thread_is_ready(&t), true);

    ok = thread_destroy(&t, &err);
    // thread may not be used after thread_destroy, unless thread_create
    // is called on it.

    assert_equal(ok, true);
    assert_equal(err.error_code, 0);
}

define_test(thread_create_recreates_destroyed_thread)
{
    thread t{};
    error err{};

    bool ok = thread_create(&t, empty_thread_func, nullptr, nullptr, &err);

    assert_equal(ok, true);
    assert_equal(err.error_code, 0);
    assert_equal(thread_is_ready(&t), true);

    ok = thread_destroy(&t, &err);

    assert_equal(ok, true);
    assert_equal(err.error_code, 0);

    ok = thread_create(&t, empty_thread_func, nullptr, nullptr, &err);

    assert_equal(ok, true);
    assert_equal(err.error_code, 0);
    assert_equal(thread_is_ready(&t), true);

    thread_destroy(&t, &err);
}

template<typename T>
void assert_equal_within_thread(T *arg, T expected)
{
    assert_equal(*arg, expected);
}

template<typename T>
void assert_not_equal_within_thread(T *arg, T unexpected)
{
    assert_not_equal(*arg, unexpected);
}

void *test_thread_func(void *_arg)
{
    s64 *arg = (s64*)_arg;

    assert_equal_within_thread(arg, (s64)0xdeadbeef);

    program_context *nctx = get_context_pointer();
    assert_not_equal_within_thread(&nctx->thread_id, 0);

    sleep_ms(500);

    return _arg;
}

define_test(thread_full_test)
{
    thread t{};
    error err{};

    s64 arg = 0xdeadbeef;
    bool ok = thread_create(&t, test_thread_func, &arg, nullptr, &err);

    assert_equal(ok, true);
    assert_equal(err.error_code, 0);
    assert_equal(thread_is_ready(&t), true);
    assert_equal(thread_is_running(&t), false);
    assert_equal(thread_is_stopped(&t), false);

    ok = thread_start(&t, &err);

    sleep_ms(200);

    s64 tid = t.thread_id;

    assert_equal(ok, true);
    assert_equal(err.error_code, 0);
    assert_equal(thread_is_ready(&t), false);
    assert_equal(thread_is_running(&t), true);
    assert_equal(thread_is_stopped(&t), false);
    assert_not_equal(tid, 0);

    ok = thread_stop(&t, nullptr, &err);

    assert_equal(thread_is_ready(&t), false);
    assert_equal(thread_is_running(&t), false);
    assert_equal(thread_is_stopped(&t), true);

    // recreate
    ok = thread_create(&t, test_thread_func, &arg, nullptr, &err);
    assert_equal(ok, true);
    assert_equal(err.error_code, 0);
    assert_equal(thread_is_ready(&t), true);
    assert_equal(thread_is_running(&t), false);
    assert_equal(thread_is_stopped(&t), false);

    thread_destroy(&t, &err);
}

define_default_test_main();
