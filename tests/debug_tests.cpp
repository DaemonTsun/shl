
#include "shl/debug.hpp"

/* commented out obviously otherwise tests crash
#include "t1/t1.hpp"
int often_called_function(int n)
{
    breakpoint(1);
    if (n > 0)
        return 1;
    return 0;
}

define_test(breakpoint_test)
{
    for (int i = 0; i < 500; ++i)
    {
        if (i == 400)
            enable_breakpoint(1);
        if (i == 403)
            disable_breakpoint(1);

        often_called_function(i);
    }
}

define_default_test_main();
*/
int main()
{
    return 0;
}
