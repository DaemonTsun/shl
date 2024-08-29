
#include "t1/t1.hpp"
#include "shl/string.hpp"
#include "shl/print.hpp"

define_test(test_1)
{
    auto uts = u8"今日は привет"_cs;

    put(uts);
    put(char_cast(L"\n"));

    tprint(u8"abc % def\n", uts);

    tprint("str length: %\n", uts.size);
    for_utf_string(i_str, i_utf, cp, c, uts)
    {
        tprint("%#2s %#2s 0x%08x %: ", i_str, i_utf, cp, c.size);
        put(c);
        put("\n");
    }

    put("\n");

    for_utf_string(cp, c, uts)
    {
        tprint("0x%08x ", cp);
        put(c);
        put("\n");
    }
}

define_default_test_main()
