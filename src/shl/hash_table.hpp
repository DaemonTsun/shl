
#pragma once

/* hash_table.hpp
 *
 * the hash table.
 *
 */

#include "shl/compare.hpp"
#include "shl/bits.hpp"
#include "shl/hash.hpp"
#include "shl/array.hpp"

#ifndef NDEBUG
#include <assert.h>
#else
#define assert(...) do {} while (0);
#endif

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

    table->hasher = hasher;
    table->eq = eq;

    for_array(v, &table->data)
        v->hash = NULL_HASH;

    table->size = 0;
}

#define _iterate_table(hash_var, table_var) \
    if (hash_var < FIRST_HASH)\
        hash_var = hash_var + FIRST_HASH;\
\
    u32 mask = table_var->data.size - 1;\
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

    if (table->data.size == 0)
        return nullptr;

    u64 max_size = table->data.size * TABLE_SIZE_FACTOR;
    u64 cur_size = (table->size * 2 + 1) * 100;

    if (cur_size >= max_size)
        expand_table(table);

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

    if (table->data.size == 0)
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

    if (table->data.size == 0)
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
    u64 I_Var = 0;\
    typename remove_pointer(decltype(TABLE))::entry_type *E_Var = (TABLE)->data.data;\
    typename remove_pointer(decltype(TABLE))::value_type *V_Var = &E_Var->value;

#define for_hash_table_V(V_Var, TABLE)\
    _for_hash_table_vars(V_Var##_index, V_Var, V_Var##_entry, TABLE)\
    for (; V_Var##_index < (TABLE)->data.size; ++V_Var##_index, ++V_Var##_entry, V_Var = &V_Var##_entry->value)\
    if (V_Var##_entry->hash >= FIRST_HASH)

#define for_hash_table_KV(K_Var, V_Var, TABLE)\
    _for_hash_table_vars(K_Var##V_Var##_index, V_Var, K_Var##V_Var##_entry, TABLE)\
    typename remove_pointer(decltype(TABLE))::key_type *K_Var = &K_Var##V_Var##_entry->key;\
    for (; K_Var##V_Var##_index < (TABLE)->data.size; ++K_Var##V_Var##_index, ++K_Var##V_Var##_entry, K_Var = &K_Var##V_Var##_entry->key, V_Var = &K_Var##V_Var##_entry->value)\
    if (K_Var##V_Var##_entry->hash >= FIRST_HASH)

#define for_hash_table_KVE(K_Var, V_Var, E_Var, TABLE)\
    _for_hash_table_vars(K_Var##V_Var##E_Var##_index, V_Var, E_Var, TABLE)\
    typename remove_pointer(decltype(TABLE))::key_type *K_Var = &E_Var->key;\
    for (; K_Var##V_Var##E_Var##_index < (TABLE)->data.size; ++K_Var##V_Var##E_Var##_index, ++E_Var, K_Var = &E_Var->key, V_Var = &E_Var->value)\
    if (E_Var->hash >= FIRST_HASH)

#define for_hash_table(...) GET_MACRO3(__VA_ARGS__, for_hash_table_KVE, for_hash_table_KV, for_hash_table_V)(__VA_ARGS__)
