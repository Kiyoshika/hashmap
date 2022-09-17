#include "hashmap.h"

static const char* alloc_err = "map_init: Couldn't allocate memory for new hashmap.";
static const char* resize_err = "map_put: Couldn't allocate memory while resizing map.";

struct hashmap_t* map_init(
    size_t capacity, 
    size_t key_size,
    size_t value_size,
    size_t (*hash)(void*),
    int (*key_compare)(const void*, const void*))
{
    struct hashmap_t* map = malloc(sizeof(struct hashmap_t));
    if (!map)
        map_err(alloc_err);
    
    map->hash = hash;
    map->key_compare = key_compare;
    map->key_size = key_size;
    map->value_size = value_size;
    
    map->max_map_capacity = capacity;
    map->unique_bins_used = 0;
    map->total_values_used = 0;
    map->bins = malloc(sizeof(struct bin_t) * map->max_map_capacity);
    if (!map->bins)
        map_err(alloc_err);
    for (size_t i = 0; i < map->max_map_capacity; ++i)
    {
        map->bins[i].max_bin_capacity = 5;
        map->bins[i].items_used = 0;
        map->bins[i].items = calloc(map->bins[i].max_bin_capacity, sizeof(struct key_value_t));
        if (!map->bins[i].items)
            map_err(alloc_err);
    }

    map->load_factor = 0.75f;
    map->load_factor_capacity = (size_t)(map->max_map_capacity * map->load_factor);

    return map;
}

static void _insert_key_value_into_bin(struct hashmap_t** map, void* key, void* value)
{
    size_t hash_index = (*map)->hash(key) % (*map)->max_map_capacity;

    if ((*map)->bins[hash_index].items_used == 0)
        (*map)->unique_bins_used++;

    // copy contents of key & value into newly-allocated pointers
    struct key_value_t* key_value = malloc(sizeof(struct key_value_t));
    key_value->key = malloc((*map)->key_size);
    key_value->value = malloc((*map)->value_size);
    memcpy(key_value->key, key, (*map)->key_size);
    memcpy(key_value->value, value, (*map)->value_size);

    // copy contents of allocated pointers into bin key & value slots
    void* base = &(*map)->bins[hash_index].items[(*map)->bins[hash_index].items_used];
    memcpy(base, key_value, sizeof(*key_value));
    memcpy(((struct key_value_t*)base)->key, key_value->key, (*map)->key_size);
    memcpy(((struct key_value_t*)base)->value, key_value->value, (*map)->value_size);
    (*map)->bins[hash_index].items_used++;
    
    if ((*map)->bins[hash_index].items_used == (*map)->bins[hash_index].max_bin_capacity)
    {
        (*map)->bins[hash_index].max_bin_capacity *= 2;
        void* alloc = realloc((*map)->bins[hash_index].items, (*map)->bins[hash_index].max_bin_capacity * sizeof(struct key_value_t));
        if (!alloc)
            map_err("map_put: Couldn't allocate memory while expanding bin size.");
        (*map)->bins[hash_index].items = alloc;
    }

    free(key_value);
}

static void _resize_map(struct hashmap_t** map)
{
    /*
     * STEPS TO RESIZE:
     * 1. copy all old keys & values previously used
     * 2. dump old map's memory and reset everything to default values (a bit heap expensive...)
     * 3. iterate over copied keys/values and rehash with new capacity
     */

    // create a copy of all the used keys & values up to this point and
    // free the old ones. this is because we'll be rehashing the keys after resizing
    // and don't want to overwrite the old pointers causing a memory leak
    struct key_value_t* used_key_values = malloc(sizeof(struct key_value_t) * (*map)->total_values_used);
    if (!used_key_values)
        map_err(resize_err);

    size_t items_used = 0;
    for (size_t i = 0; i < (*map)->max_map_capacity; ++i)
    {
        for (size_t b = 0; b < (*map)->bins[i].items_used; ++b)
        {
            used_key_values[items_used].key = malloc((*map)->key_size);
            if (!used_key_values[items_used].key)
                map_err(resize_err);
            memcpy(used_key_values[items_used].key, (*map)->bins[i].items[b].key, (*map)->key_size);
            free((*map)->bins[i].items[b].key);
            //(*map)->bins[i].items[b].key = NULL;

            used_key_values[items_used].value = malloc((*map)->value_size);
            if (!used_key_values[items_used].value)
                map_err(resize_err);
            memcpy(used_key_values[items_used].value, (*map)->bins[i].items[b].value, (*map)->value_size);
            free((*map)->bins[i].items[b].value);
            //(*map)->bins[i].items[b].value = NULL;

            items_used++;
        }
        free((*map)->bins[i].items);
    }
    free((*map)->bins);

    // double the capacity of map (bins)
    (*map)->max_map_capacity *= 2;
    (*map)->bins = malloc((*map)->max_map_capacity * sizeof(struct bin_t));
    if (!(*map)->bins)
        map_err(resize_err);
    for (size_t i = 0; i < (*map)->max_map_capacity; ++i)
    {
        (*map)->bins[i].max_bin_capacity = 5;
        (*map)->bins[i].items_used = 0;
        (*map)->bins[i].items = calloc((*map)->bins[i].max_bin_capacity, sizeof(struct key_value_t));
        if (!(*map)->bins[i].items)
            map_err(resize_err);
    }

    // update load factor
    (*map)->load_factor_capacity = (size_t)((*map)->load_factor * (*map)->max_map_capacity);

    // reset total items used and bins used and start rehashing keys
    size_t old_total_values_used = (*map)->total_values_used;
    (*map)->total_values_used = 0;
    (*map)->unique_bins_used = 0;
    
    for (size_t i = 0; i < old_total_values_used; ++i)
    {
        map_put(map, used_key_values[i].key, used_key_values[i].value);
        free(used_key_values[i].key);
        free(used_key_values[i].value);
    }
    free(used_key_values);
}

struct key_value_t* map_get(const struct hashmap_t* map, void* key)
{
    size_t hash_index = map->hash(key) % map->max_map_capacity;
    struct bin_t get_bin = map->bins[hash_index];
    for (size_t i = 0; i < get_bin.items_used; ++i)
    {
        if (map->key_compare(get_bin.items[i].key, key) == 0)
            return &get_bin.items[i];
    }

    return NULL;
}

static bool _check_duplicate_key(const struct hashmap_t* map, void* key)
{
    struct key_value_t* kv = map_get(map, key);
    if (kv)
        return true;
    
    return false;
}

enum map_code_e map_put(struct hashmap_t** map, void* key, void* value)
{
    if (_check_duplicate_key(*map, key))
        return DUPLICATE_KEY;

    _insert_key_value_into_bin(map, key, value);
    (*map)->total_values_used++;
    if ((*map)->unique_bins_used == (*map)->load_factor_capacity)
        _resize_map(map);
    
    return SUCCESS;
}

void map_free(struct hashmap_t** map)
{

    for (size_t i = 0; i < (*map)->max_map_capacity; ++i)
    {
        for (size_t b = 0; b < (*map)->bins[i].items_used; ++b)
        {
            free((*map)->bins[i].items[b].key);
            //(*map)->bins[i].items[b].key = NULL;

            free((*map)->bins[i].items[b].value);
            //(*map)->bins[i].items[b].value = NULL;

        }
    }

    for (size_t i = 0; i < (*map)->max_map_capacity; ++i)
        free((*map)->bins[i].items);

    free((*map)->bins);
    (*map)->bins = NULL;

    free(*map);
    *map = NULL;
}