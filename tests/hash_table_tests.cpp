
#include <t1/t1.hpp>
#include "shl/hash_table.hpp"

define_test(hash_table_init_initializes)
{
    hash_table<u32, const char *> table;
    init(&table);

    assert_equal(table.size, 0);

    free(&table);
}

define_default_test_main();
