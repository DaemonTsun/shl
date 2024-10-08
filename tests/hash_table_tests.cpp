
#include <t1/t1.hpp>
#include "shl/string.hpp"
#include "shl/hash_table.hpp"

define_test(init_initializes_hash_table)
{
    hash_table<u32, const char *> table;
    init(&table);

    assert_equal(table.size, 0);

    free(&table);
}

define_test(init_for_n_items_initializes_hash_table)
{
    hash_table<int, const char *> table;
    init_for_n_items(&table, 4);

    s64 start_reserved_size = table.data.size;
    assert_equal(table.size, 0);

    table[0] = "a";

    assert_equal(table.size, 1);
    assert_equal(table.data.size, start_reserved_size);

    table[1] = "b";

    assert_equal(table.size, 2);
    assert_equal(table.data.size, start_reserved_size);

    table[2] = "c";

    assert_equal(table.size, 3);
    assert_equal(table.data.size, start_reserved_size);

    table[3] = "d";

    assert_equal(table.size, 4);
    assert_equal(table.data.size, start_reserved_size);

    /*
    table[5] = "a";
    from this point on you're not guaranteed that table will or will not resize
    */

    free(&table);
}

define_test(add_element_by_key_adds_key_to_table_and_returns_pointer_to_value)
{
    hash_table<u32, const char *> table{};

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
    hash_table<u32, const char *> table{};

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
    hash_table<u32, const char *> table{};

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
    hash_table<string, string> table{};

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
    hash_table<u32, const char *> table{};

    assert_equal(table.size, 0);

    u32 key = 5;

    const char **v = add_element_by_key(&table, &key);

    assert_not_equal(v, nullptr);
    assert_equal(table.size, 1);

    *v = "hello";

    const char **res = search(&table, &key);

    assert_not_equal(res, nullptr);
    assert_equal(res, v);
    assert_equal(string_compare(*res, "hello"), 0);

    free(&table);
}

define_test(search_returns_nullptr_if_no_match_was_found)
{
    hash_table<u32, const char *> table{};

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
    hash_table<string, u32> table{};

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

    u32 *v4 = search_by_hash(&table, "abc"_hash); // _hash string literal hash operator
    assert_equal(v4, v);
    assert_equal(v4, v2);
    assert_equal(v4, v3);

    free(&table);
    free(&key);
}

define_test(search_or_insert_returns_pointer_to_inserted_element)
{
    hash_table<u32, const char *> table{};

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

    assert_equal(string_compare(*v2, "hello"), 0);

    free(&table);
}

define_test(contains_returns_whether_key_is_contained_in_hash_table)
{
    hash_table<u32, const char *> table{};

    u32 key = 5;
    u32 key2 = 10;
    add_element_by_key(&table, &key);

    assert_equal(contains(&table, &key), true);
    assert_equal(contains(&table, &key2), false);

    free(&table);
}

define_test(clear_clears_hash_table)
{
    hash_table<u32, const char *> table{};

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
    hash_table<u32, const char *> table{};

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
    hash_table<string, int> table{};

    // string allocates memory
    table["hello"_s] = 5;

    assert_equal(table.size, 1);

    // <true> frees keys as well
    free<true>(&table);
}

define_test(free_can_free_values)
{
    hash_table<int, string> table{};

    table[5] = "hello"_s;

    assert_equal(table.size, 1);

    // <..., true> frees values as well
    free<false, true>(&table);
}

define_test(free_can_free_keys_and_values)
{
    hash_table<string, string> table{};

    table["abc"_s] = "hello"_s;

    assert_equal(table.size, 1);

    free<true, true>(&table);
}

define_test(remove_element_by_key_can_free_keys_and_values)
{
    hash_table<string, string> table{};

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
    hash_table<int, int> table{};

    assert_equal(table.size, 0);
    assert_greater_or_equal(table.data.reserved_size, 0);
    
    init(&table);

    assert_equal(table.size, 0);
    assert_greater_or_equal(table.data.reserved_size, MIN_TABLE_SIZE);

    for (int i = 0; i < MIN_TABLE_SIZE; ++i)
        add_element_by_key(&table, &i);

    assert_equal(table.size, MIN_TABLE_SIZE);
    assert_greater_or_equal(table.data.reserved_size, MIN_TABLE_SIZE * 2);

    for (int i = MIN_TABLE_SIZE; i < (MIN_TABLE_SIZE * 2); ++i)
        add_element_by_key(&table, &i);

    assert_equal(table.size, MIN_TABLE_SIZE * 2);
    assert_greater_or_equal(table.data.reserved_size, MIN_TABLE_SIZE * 4);

    free(&table);
}

define_test(equality_operator_checks_hash_table_equality)
{
    hash_table<u32, u32> table1{};
    hash_table<u32, u32> table2{};

    assert_equal(table1, table2);

    table1[10] = 10;

    assert_not_equal(table1, table2);

    table2[5] = 5;

    assert_not_equal(table1, table2);

    table1[5] = 5;
    table2[10] = 10;

    assert_equal(table1, table2);

    free(&table1);
    free(&table2);
}

define_default_test_main();
