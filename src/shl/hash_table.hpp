
#pragma once

/* hash_table.hpp

the hash table.

A pretty simple hash table that stores everything in an array and marks
entries as unused/removed/used by setting the hash in the entry.
Hash collisions are dealt with by simply advancing to the next unoccupied slot
in the array. For this, the keys are compared using a comparator function.
After enough entries are occupied in the hash_table, the table expands
automatically on insert.

The index operator works the same as search_or_insert, except it also accepts
references and returns a reference to the element instead, e.g.:
    
    hash_table<int, const char*> table;
    init(&table);
    table[5] = "hello";
    free(&table);

hash_table.size is the number of currently used entries in the table.
hash_table.data is the array with all entires, unused and used.
hash_table.hasher is the hash function used to hash the keys.
hash_table.eq is the equality function used to compare the keys when hashes
match.

add_element_by_key(*table, *K) returns a pointer to a newly added
    element with key K and hash table->hasher(K).
    expands the table automatically when necessary.
    does not compare keys on collision and simply adds a new entry.

remove_element_by_key(*table, *K) removes the given entry with key K
    from the table. Returns whether it was successful or not.
    No hash table memory is deallocated and the entry is simply marked
    "removed".
    Optional template parameters FreeKey and FreeValue can be used to
    call free() on the key or value of the removed entry.

remove_element_by_hash(*table, H) same thing as remove_element_by_key,
    but removes the element of hash H instead.

expand_table(*table) expands the table. used internally.

search(*table, *K) returns a pointer to the element in the table that has
    the same hash as table->hasher(K) and the same key K.
    If no entry is found, returns nullptr.
    Does not modify the table.

search_by_hash(*table, H) same thing as search(), but looks up only the
    first entry matching the hash H.

search_or_insert(*table, *K) returns a pointer to the element in the table
    that has the same hash as table->hasher(K) and the same key K, or
    if none was found, a pointer to a newly inserted element that has the
    key K and the hash table->hasher(K).
    Should never return nullptr.

contains(*table, *K) returns whether the table contains an element with the
    key K.

contains_hash(*table, H) returns whether the table contains an element with
    the hash H.

clear(*table) marks all entries as unused and sets table size to 0.
    Does not deallocate table memory.

free(*table) deallocates the tables memory.

for_hash_table(*value, *table)
for_hash_table(*key, *value, *table)
for_hash_table(*key, *value, *entry, *table)
    iterates the table. order is not guaranteed. example:

    hash_table<int, const char*> table;
    init(&table);
    table[5] = "hello";
    table[10] = "world";
    table[100] = "abc";

    for_hash_table(key, value, entry, &table)
        printf("%d: %s\n", *key, *value);

    free(&table);
*/

#include <assert.h>
#include "shl/compare.hpp"
#include "shl/bits.hpp"
#include "shl/hash.hpp"
#include "shl/array.hpp"
#include "shl/macros.hpp"

#define TABLE_SIZE_FACTOR 75
#define MIN_TABLE_SIZE 64
#define NULL_HASH 0
#define REMOVED_HASH 1
#define FIRST_HASH 2

template<typename TKey, typename TValue>
struct hash_table_entry
{
    hash_t hash;
    TKey key;
    TValue value;
};

template<typename TKey, typename TValue>
struct hash_table
{
    typedef TKey key_type;
    typedef TValue value_type;
    typedef hash_table_entry<TKey, TValue> entry_type;

    array<entry_type> data;
    u64 size;

    hash_function<TKey> hasher;
    equality_function_p<TKey> eq;

    TValue &operator[](const TKey &key) { return *search_or_insert(this, &key); }
    TValue &operator[](const TKey *key) { return *search_or_insert(this, key); }
};

template<typename TKey, typename TValue>
void init(hash_table<TKey, TValue> *table, u64 initial_size = MIN_TABLE_SIZE, hash_function<TKey> hasher = hash, equality_function_p<TKey> eq = equals_p<TKey>)
{
    assert(table != nullptr);

    if (initial_size == 0)
        initial_size = MIN_TABLE_SIZE;
    else
        initial_size = ceil_exp2(initial_size);

    init(&table->data, initial_size);

    if (hasher == nullptr)
        hasher = hash;

    table->hasher = hasher;

    if (eq == nullptr)
        eq = equals_p<TKey>;

    table->eq = eq;

    for_array(v, &table->data)
        v->hash = NULL_HASH;

    table->size = 0;
}

#define _iterate_table(hash_var, table_var) \
    if (hash_var < FIRST_HASH)\
        hash_var = hash_var + FIRST_HASH;\
\
    u32 mask = (u32)(table_var->data.size - 1);\
    u32 _inc = 1;\
    u32 index = hash_var & mask;\
    while (table_var->data[index].hash != NULL_HASH)

#define _advance_table_it() \
    index += _inc;\
    index = index & mask;\
    _inc++;

template<typename TKey, typename TValue>
TValue *add_element_by_key(hash_table<TKey, TValue> *table, const TKey *key)
{
    assert(table != nullptr);
    assert(key != nullptr);

    if (table->data.data == nullptr)
    {
        init(table, MIN_TABLE_SIZE, table->hasher, table->eq);
    }
    else
    {
        u64 max_size = table->data.size * TABLE_SIZE_FACTOR;
        u64 cur_size = (table->size * 2 + 1) * 100;

        if (cur_size >= max_size)
            expand_table(table);
    }

    hash_t hsh = table->hasher(key);

    _iterate_table(hsh, table)
    {
        _advance_table_it();
    }

    hash_table_entry<TKey, TValue> *entry = at(&table->data, index);
    table->size++;
    entry->hash = hsh;
    entry->key = *key;

    return &entry->value;
}

template<bool FreeKey = false, bool FreeValue = false, typename TKey, typename TValue>
bool remove_element_by_key(hash_table<TKey, TValue> *table, const TKey *key)
{
    assert(table != nullptr);
    assert(key != nullptr);

    if (table->data.data == nullptr || table->data.size == 0)
        return false;

    hash_t hsh = table->hasher(key);

    _iterate_table(hsh, table)
    {
        hash_table_entry<TKey, TValue> *ent = at(&table->data, index);

        if (ent->hash == hsh && table->eq(key, &ent->key))
        {
            ent->hash = REMOVED_HASH;

            if constexpr (FreeKey)   free(&ent->key);
            if constexpr (FreeValue) free(&ent->value);
            table->size--;
            return true;
        }

        _advance_table_it();
    }

    return false;
}

template<bool FreeKey = false, bool FreeValue = false, typename TKey, typename TValue>
bool remove_element_by_hash(hash_table<TKey, TValue> *table, hash_t hsh)
{
    assert(table != nullptr);

    if (table->data.data == nullptr || table->data.size == 0)
        return false;

    _iterate_table(hsh, table)
    {
        hash_table_entry<TKey, TValue> *ent = at(&table->data, index);

        if (ent->hash == hsh) // && table->eq(key, &ent->key))
        {
            ent->hash = REMOVED_HASH;

            if constexpr (FreeKey)   free(&ent->key);
            if constexpr (FreeValue) free(&ent->value);
            table->size--;
            return true;
        }

        _advance_table_it();
    }

    return false;
}

template<typename TKey, typename TValue>
void expand_table(hash_table<TKey, TValue> *table)
{
    assert(table != nullptr);

    if (table->data.data == nullptr)
    {
        // I suppose initializing is expanding
        init(table, MIN_TABLE_SIZE, table->hasher, table->eq);
        return;
    }

    u64 new_size = table->data.size;
    u64 max_size = table->data.size * TABLE_SIZE_FACTOR;
    u64 cur_size = (table->size * 2 + 1) * 100;

    if (cur_size > max_size)
        new_size = new_size * 2;

    if (new_size < MIN_TABLE_SIZE)
        new_size = MIN_TABLE_SIZE;

    array<hash_table_entry<TKey, TValue>> old_entries = table->data;
    init(&table->data, new_size);

    for_array(nv, &table->data)
        nv->hash = NULL_HASH;

    table->size = 0;

    for_array(v, &old_entries)
        if (v->hash >= FIRST_HASH)
        {
            TValue *added_val = add_element_by_key(table, &v->key);
            *added_val = v->value;
        }

    free(&old_entries);
}

template<typename TKey, typename TValue>
TValue *search(hash_table<TKey, TValue> *table, const TKey *key)
{
    assert(table != nullptr);
    assert(key != nullptr);

    if (table->data.size == 0)
        return nullptr;

    hash_t hsh = table->hasher(key);

    _iterate_table(hsh, table)
    {
        hash_table_entry<TKey, TValue> *ent = at(&table->data, index);

        if (ent->hash == hsh && table->eq(key, &ent->key))
            return &ent->value;

        _advance_table_it();
    }

    return nullptr;
}

template<typename TKey, typename TValue>
TValue *search_by_hash(hash_table<TKey, TValue> *table, hash_t hsh)
{
    assert(table != nullptr);

    if (table->data.size == 0)
        return nullptr;

    _iterate_table(hsh, table)
    {
        hash_table_entry<TKey, TValue> *ent = at(&table->data, index);

        if (ent->hash == hsh) // && table->eq(key, &ent->key))
            return &ent->value;

        _advance_table_it();
    }

    return nullptr;
}

template<typename TKey, typename TValue>
TValue *search_or_insert(hash_table<TKey, TValue> *table, const TKey *key)
{
    TValue *v = search(table, key);
    
    if (v != nullptr)
        return v;
    
    return add_element_by_key(table, key);
}

template<typename TKey, typename TValue>
bool contains(hash_table<TKey, TValue> *table, const TKey *key)
{
    return search(table, key) != nullptr;
}

template<typename TKey, typename TValue>
bool contains_hash(hash_table<TKey, TValue> *table, hash_t hash)
{
    return search_by_hash(table, hash) != nullptr;
}

template<typename TKey, typename TValue>
void clear(hash_table<TKey, TValue> *table)
{
    assert(table != nullptr);

    for_array(v, &table->data)
        v->hash = NULL_HASH;

    table->size = 0;
}

template<bool FreeKeys = false, bool FreeValues = false, typename TKey, typename TValue>
void free(hash_table<TKey, TValue> *table)
{
    assert(table != nullptr);

    if constexpr (FreeKeys || FreeValues)
    {
        for_array(v, &table->data)
        {
            if (v->hash < FIRST_HASH)
                continue;

            if constexpr (FreeKeys)   free(&v->key);
            if constexpr (FreeValues) free(&v->value);
        }
    }

    free(&table->data);

    table->size = 0;
}

#define _for_hash_table_vars(I_Var, V_Var, E_Var, TABLE)\
    if constexpr (u64 I_Var = 0; true)\
    if constexpr (auto *E_Var = (TABLE)->data.data; true)\
    if constexpr (auto *V_Var = &E_Var->value; true)

#define for_hash_table_V(V_Var, TABLE)\
    _for_hash_table_vars(V_Var##_index, V_Var, V_Var##_entry, TABLE)\
    for (; V_Var##_index < (TABLE)->data.size; ++V_Var##_index, ++V_Var##_entry, V_Var = &V_Var##_entry->value)\
    if (V_Var##_entry->hash >= FIRST_HASH)

#define for_hash_table_KV(K_Var, V_Var, TABLE)\
    _for_hash_table_vars(K_Var##V_Var##_index, V_Var, K_Var##V_Var##_entry, TABLE)\
    if constexpr (auto *K_Var = &K_Var##V_Var##_entry->key; true)\
    for (; K_Var##V_Var##_index < (TABLE)->data.size; ++K_Var##V_Var##_index, ++K_Var##V_Var##_entry, K_Var = &K_Var##V_Var##_entry->key, V_Var = &K_Var##V_Var##_entry->value)\
    if (K_Var##V_Var##_entry->hash >= FIRST_HASH)

#define for_hash_table_KVE(K_Var, V_Var, E_Var, TABLE)\
    _for_hash_table_vars(K_Var##V_Var##E_Var##_index, V_Var, E_Var, TABLE)\
    if constexpr (auto *K_Var = &E_Var->key; true)\
    for (; K_Var##V_Var##E_Var##_index < (TABLE)->data.size; ++K_Var##V_Var##E_Var##_index, ++E_Var, K_Var = &E_Var->key, V_Var = &E_Var->value)\
    if (E_Var->hash >= FIRST_HASH)

#define for_hash_table(...) GET_MACRO3(__VA_ARGS__, for_hash_table_KVE, for_hash_table_KV, for_hash_table_V)(__VA_ARGS__)

template<typename TKey, typename TValue>
bool operator==(hash_table<TKey, TValue> &lhs, hash_table<TKey, TValue> &rhs)
{
    if (lhs.size != rhs.size)
        return false;

    for_hash_table(key, val, &lhs)
    {
        TValue *other = search(&rhs, key);

        if (other == nullptr)
            return false;

        if (!(*other == *val))
            return false;
    }

    return true;
}

template<typename TKey, typename TValue>
bool operator!=(hash_table<TKey, TValue> &lhs, hash_table<TKey, TValue> &rhs)
{
    return !(lhs == rhs);
}
