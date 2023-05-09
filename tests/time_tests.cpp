
#include <t1/t1.hpp>
#include "shl/time.hpp"

define_test(get_seconds_difference_gets_difference_in_seconds)
{
    timespan start;
    get_time(&start);

    sleep(1.0);

    timespan end;
    get_time(&end);

    double diff = get_seconds_difference(&start, &end);

    assert_greater_or_equal(diff, 1.0);
}

define_default_test_main();
