
#include "t1/t1.hpp"
#include "shl/macros.hpp"

define_test(offset_of_yields_offset_of_struct_member)
{
    struct somestruct
    {
        s8  a;
        s8  b;
        s16 c;
        s32 d;
    };

    assert_equal((int)offset_of(somestruct, a), 0);
    assert_equal((int)offset_of(somestruct, b), 1);
    assert_equal((int)offset_of(somestruct, c), 2);
    assert_equal((int)offset_of(somestruct, d), 4);
}

define_default_test_main()
