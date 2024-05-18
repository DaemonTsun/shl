
#define t1_use_stdlib 1
#include <t1/t1.hpp>
#include "shl/time.hpp"
#include "shl/platform.hpp"

define_test(get_seconds_difference_gets_difference_in_seconds)
{
    timespan start;
    get_time(&start);

    sleep(1.0);

    timespan end;
    get_time(&end);

    double diff = get_seconds_difference(&start, &end);

#if Windows
    assert_greater_or_equal(diff, 0.9f);
#else
    assert_greater_or_equal(diff, 1.0);
#endif
}

define_default_test_main();
