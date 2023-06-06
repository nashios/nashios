#pragma once

#include <st/assert.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef KERNEL
#include <Kernel/api/posix/errno.h>
#include <Kernel/lib/stdlib.h>
#include <Kernel/lib/string.h>
#else
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#endif

#define HASHMAP_PROBE_LEN(map) ((map)->table_size >> 1)
#define HASHMAP_SIZE_MOD(map, val) ((val) & ((map)->table_size - 1))
#define HASHMAP_SIZE_DEFAULT (1 << 8)
#define HASHMAP_SIZE_MIN (1 << 5)
#define HASHMAP_PROBE_NEXT(map, index) HASHMAP_SIZE_MOD(map, (index) + 1)

struct hashmap_entry
{
    void *key;
    void *data;
};

struct hashmap
{
    size_t table_size_init;
    size_t table_size;
    size_t num_entries;
    struct hashmap_entry *table;

    size_t (*hash)(const void *key);
    int (*key_compare)(const void *, const void *);
    void *(*key_alloc)(const void *);
    void (*key_free)(void *);
};

typedef size_t (*hashmap_hash_func_t)(const void *);
typedef int (*hashmap_key_compare_func_t)(const void *, const void *);

static inline size_t hashmap_hash_uint32(const void *key) { return *(uint32_t *)key; }

static inline int hashmap_compare_uint32(const void *a, const void *b) { return *(int32_t *)a - *(int32_t *)b; }

static inline size_t hashmap_hash_string(const void *key)
{
    const char *key_str = (const char *)key;
    size_t hash = 0;

    for (; *key_str; ++key_str)
    {
        hash += *key_str;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

static inline int hashmap_compare_string(const void *a, const void *b)
{
    return strcmp((const char *)a, (const char *)b);
}

static inline size_t hashmap_table_min_size_calc(size_t num_entries) { return num_entries + (num_entries / 3); }

static inline size_t hashmap_table_size_calc(size_t num_entries)
{
    size_t table_size = hashmap_table_min_size_calc(num_entries);
    size_t min_size = HASHMAP_SIZE_MIN;
    while (min_size < table_size)
        min_size <<= 1;
    return min_size;
}

static inline int hashmap_init(struct hashmap *map, hashmap_hash_func_t hash_func,
                               hashmap_key_compare_func_t key_compare_func, size_t initial_size)
{
    assert(map != NULL);

    if (!initial_size)
        initial_size = HASHMAP_SIZE_DEFAULT;
    else
        initial_size = hashmap_table_size_calc(initial_size);

    map->table_size_init = initial_size;
    map->table_size = initial_size;
    map->num_entries = 0;
    map->table = (struct hashmap_entry *)calloc(initial_size, sizeof(struct hashmap_entry));
    if (!map->table)
        return -ENOMEM;

    map->hash = hash_func ? hash_func : hashmap_hash_string;
    map->key_compare = key_compare_func ? key_compare_func : hashmap_compare_string;
    map->key_alloc = NULL;
    map->key_free = NULL;

    return 0;
}

static inline size_t hashmap_calc_index(const struct hashmap *map, const void *key)
{
    return HASHMAP_SIZE_MOD(map, map->hash(key));
}

static inline struct hashmap_entry *hashmap_entry_find(const struct hashmap *map, const void *key, bool find_empty)
{
    size_t probe_len = HASHMAP_PROBE_LEN(map);
    size_t index = hashmap_calc_index(map, key);

    for (size_t i = 0; i < probe_len; i++)
    {
        struct hashmap_entry *entry = &map->table[index];
        if (!entry->key)
        {
            if (find_empty)
                return entry;
            return NULL;
        }

        if (map->key_compare(key, entry->key) == 0)
            return entry;

        index = HASHMAP_PROBE_NEXT(map, index);
    }
    return NULL;
}

static inline void *hashmap_get(const struct hashmap *map, const void *key)
{
    assert(map != NULL);
    assert(key != NULL);

    struct hashmap_entry *entry = hashmap_entry_find(map, key, false);
    if (!entry)
        return NULL;
    return entry->data;
}

static inline int hashmap_rehash(struct hashmap *map, size_t new_size)
{
    assert(new_size >= HASHMAP_SIZE_MIN);
    assert((new_size & (new_size - 1)) == 0);

    struct hashmap_entry *new_table = (struct hashmap_entry *)calloc(new_size, sizeof(struct hashmap_entry));
    if (!new_table)
        return -ENOMEM;

    size_t old_size = map->table_size;
    struct hashmap_entry *old_table = map->table;
    map->table_size = new_size;
    map->table = new_table;

    for (struct hashmap_entry *entry = old_table; entry < &old_table[old_size]; ++entry)
    {
        if (!entry->data)
            continue;

        struct hashmap_entry *new_entry = hashmap_entry_find(map, entry->key, true);
        if (!new_entry)
            goto revert;

        new_entry->key = entry->key;
        new_entry->data = entry->data;
    }

    free(old_table);
    return 0;

revert:
    map->table_size = old_size;
    map->table = old_table;
    free(new_table);

    return -EINVAL;
}

static inline void *hashmap_put(struct hashmap *map, const void *key, void *data)
{
    assert(map != NULL);
    assert(key != NULL);

    if (map->table_size <= hashmap_table_min_size_calc(map->num_entries))
        hashmap_rehash(map, map->table_size << 1);

    struct hashmap_entry *entry = hashmap_entry_find(map, key, true);
    if (!entry)
    {
        if (hashmap_rehash(map, map->table_size << 1) < 0)
            return NULL;

        entry = hashmap_entry_find(map, key, true);
        if (!entry)
            return NULL;
    }

    if (!entry->key)
    {
        if (map->key_alloc)
        {
            entry->key = map->key_alloc(key);
            if (!entry->key)
                return NULL;
        }
        else
            entry->key = (void *)key;

        map->num_entries++;
    }
    else if (entry->data)
        return entry->data;

    entry->data = data;
    return data;
}
