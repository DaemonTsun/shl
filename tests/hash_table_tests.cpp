
#include <t1/t1.hpp>
#include "shl/string.hpp"
#include "shl/hash_table.hpp"

define_test(hash_table_init_initializes)
{
    hash_table<u32, const char *> table;
    init(&table);

    assert_equal(table.size, 0);

    free(&table);
}

define_test(add_element_by_key_adds_key_to_table_and_returns_pointer_to_value)
{
    hash_table<u32, const char *> table;
    init(&table);

    assert_equal(table.size, 0);

    u32 key = 5;
    const char **v = add_element_by_key(&table, &key);

    assert_not_equal(v, nullptr);
    assert_equal(table.size, 1);

    *v = "hello";

    // adds the same key regardless of if its already contained in the
    // table.
    const char **v2 = add_element_by_key(&table, &key);

    assert_not_equal(v2, nullptr);
    assert_not_equal(v, v2);
    assert_equal(table.size, 2);

    free(&table);
}

define_test(remove_element_by_key_removes_element_by_key_and_returns_whether_it_was_successful)
{
    hash_table<u32, const char *> table;
    init(&table);

    assert_equal(table.size, 0);

    u32 key = 5;

    assert_equal(remove_element_by_key(&table, &key), false);

    const char **v = add_element_by_key(&table, &key);

    assert_not_equal(v, nullptr);
    assert_equal(table.size, 1);

    assert_equal(remove_element_by_key(&table, &key), true);
    assert_equal(table.size, 0);
    assert_equal(remove_element_by_key(&table, &key), false);

    free(&table);
}

define_test(remove_element_by_key_removes_element_by_key_and_returns_whether_it_was_successful2)
{
    hash_table<u32, const char *> table;
    init(&table);

    assert_equal(table.size, 0);

    u32 key = 5;

    assert_equal(remove_element_by_key(&table, &key), false);

    add_element_by_key(&table, &key);
    add_element_by_key(&table, &key);

    assert_equal(table.size, 2);

    assert_equal(remove_element_by_key(&table, &key), true);
    assert_equal(table.size, 1);
    assert_equal(remove_element_by_key(&table, &key), true);
    assert_equal(table.size, 0);
    assert_equal(remove_element_by_key(&table, &key), false);

    free(&table);
}

define_test(remove_element_by_hash_removes_element_by_hash)
{
    hash_table<string, string> table;
    init(&table);

    string key = "abc"_s;

    bool ret = remove_element_by_hash<true, true>(&table, hash(key));
    assert_equal(ret, false);

    *add_element_by_key(&table, &key) = "hello"_s;

    ret = remove_element_by_hash<true, true>(&table, hash(key));
    assert_equal(ret, true);
    assert_equal(table.size, 0);

    ret = remove_element_by_hash<true, true>(&table, hash("abc"_cs));
    assert_equal(ret, false);

    free(&table);
}

define_test(search_looks_up_first_match_if_hash_is_contained_in_hash_table)
{
    hash_table<u32, const char *> table;
    init(&table);

    assert_equal(table.size, 0);

    u32 key = 5;

    const char **v = add_element_by_key(&table, &key);

    assert_not_equal(v, nullptr);
    assert_equal(table.size, 1);

    *v = "hello";

    const char **res = search(&table, &key);

    assert_not_equal(res, nullptr);
    assert_equal(res, v);
    assert_equal(compare_strings(*res, "hello"), 0);

    free(&table);
}

define_test(search_returns_nullptr_if_no_match_was_found)
{
    hash_table<u32, const char *> table;
    init(&table);

    assert_equal(table.size, 0);

    u32 key = 5;
    u32 key2 = 10;

    assert_equal(search(&table, &key), nullptr);
    assert_equal(search(&table, &key2), nullptr);

    add_element_by_key(&table, &key);
    assert_equal(table.size, 1);

    assert_not_equal(search(&table, &key), nullptr);
    assert_equal(search(&table, &key2), nullptr);

    free(&table);
}

define_test(search_by_hash_searches_entry_by_hash)
{
    hash_table<string, u32> table;
    init(&table);

    assert_equal(table.size, 0);

    string key = "abc"_s;
    table[key] = 5;
    assert_equal(table.size, 1);

    u32 *v = search(&table, &key);
    assert_not_equal(v, nullptr);

    u32 *v2 = search_by_hash(&table, hash(&key));
    assert_not_equal(v2, nullptr);
    assert_equal(v2, v);

    u32 *v3 = search_by_hash(&table, hash("abc"_cs));
    assert_equal(v3, v);
    assert_equal(v3, v2);

    u32 *v4 = search_by_hash(&table, "abc"_h); // _h string literal hash operator
    assert_equal(v4, v);
    assert_equal(v4, v2);
    assert_equal(v4, v3);

    free(&table);
    free(&key);
}

define_test(search_or_insert_returns_pointer_to_inserted_element)
{
    hash_table<u32, const char *> table;
    init(&table);

    assert_equal(table.size, 0);

    u32 key = 5;
    const char **v = search_or_insert(&table, &key);

    assert_not_equal(v, nullptr);
    assert_equal(table.size, 1);

    *v = "hello";

    const char **v2 = search_or_insert(&table, &key);

    assert_not_equal(v2, nullptr);
    assert_equal(table.size, 1);
    assert_equal(v, v2);

    assert_equal(compare_strings(*v2, "hello"), 0);

    free(&table);
}

define_test(contains_returns_whether_key_is_contained_in_hash_table)
{
    hash_table<u32, const char *> table;
    init(&table);

    u32 key = 5;
    u32 key2 = 10;
    add_element_by_key(&table, &key);

    assert_equal(contains(&table, &key), true);
    assert_equal(contains(&table, &key2), false);

    free(&table);
}

define_test(clear_clears_hash_table)
{
    hash_table<u32, const char *> table;
    init(&table);

    u32 key = 5;
    add_element_by_key(&table, &key);
    add_element_by_key(&table, &key);
    add_element_by_key(&table, &key);
    add_element_by_key(&table, &key);
    add_element_by_key(&table, &key);

    assert_equal(table.size, 5);

    clear(&table);

    assert_equal(table.size, 0);
    assert_equal(contains(&table, &key), false);

    free(&table);
}

define_test(for_hash_table_iterates_hash_table)
{
    hash_table<u32, const char *> table;
    init(&table);

    u32 key = 5;
    u32 key2 = 10;

    add_element_by_key(&table, &key);
    add_element_by_key(&table, &key);
    add_element_by_key(&table, &key2);

    assert_equal(table.size, 3);

    int i = 0;

    for_hash_table(v, &table)
        i++;

    assert_equal(i, table.size);

    for_hash_table(k, v2, &table)
        assert_equal(*k == key || *k == key2, true)

    free(&table);
}

define_test(free_can_free_keys)
{
    hash_table<string, int> table; 
    init(&table);

    // string allocates memory
    table["hello"_s] = 5;

    assert_equal(table.size, 1);

    // <true> frees keys as well
    free<true>(&table);
}

define_test(free_can_free_values)
{
    hash_table<int, string> table; 
    init(&table);

    table[5] = "hello"_s;

    assert_equal(table.size, 1);

    // <..., true> frees values as well
    free<false, true>(&table);
}

define_test(free_can_free_keys_and_values)
{
    hash_table<string, string> table; 
    init(&table);

    table["abc"_s] = "hello"_s;

    assert_equal(table.size, 1);

    free<true, true>(&table);
}

define_test(remove_element_by_key_can_free_keys_and_values)
{
    hash_table<string, string> table; 
    init(&table);

    string key = "abc"_s;
    table["abc"_s] = "hello"_s;

    assert_equal(table.size, 1);

    bool ret = remove_element_by_key<true, true>(&table, &key);
    assert_equal(ret, true);
    assert_equal(table.size, 0);

    ret = remove_element_by_key<true, true>(&table, &key);
    assert_equal(ret, false);

    free(&key);
    free(&table);
}

define_test(add_element_by_key_expands_table)
{
    hash_table<int, int> table; 
    init(&table, 64);

    assert_equal(table.size, 0);
    assert_greater_or_equal(table.data.reserved_size, 64);

    for (int i = 0; i < 64; ++i)
        add_element_by_key(&table, &i);

    assert_equal(table.size, 64);
    assert_greater_or_equal(table.data.reserved_size, 128);

    for (int i = 64; i < 128; ++i)
        add_element_by_key(&table, &i);

    assert_equal(table.size, 128);
    assert_greater_or_equal(table.data.reserved_size, 256);

    free(&table);
}

define_default_test_main();
