#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "util.h"
#include "hash_function.h"

enum map_code_e
{
    DUPLICATE_KEY = -1,
    SUCCESS = 0
};

// key-value pair to insert into bin_t.items
struct key_value_t
{
    void* key;
    void* value;
};

// bins within a hashmap that contain continguous blocks of memory to handle collisions
struct bin_t
{
    struct key_value_t* items; // chain of items within a bin within the map
    size_t items_used; // total items present in the chain
    size_t max_bin_capacity; // total possible items (resizable)
};

struct hashmap_t
{
    size_t (*hash)(void*); // hash function for the key (defaults are in hash_function.h)
    int (*key_compare)(const void*, const void*); // key comparison function used when searching bin chains after key collision
    size_t key_size; // size of individual key
    size_t value_size; // size of individual value

    struct bin_t* bins; // individual bins
    size_t total_values_used; // total items inserted into map (counts items nested within chains)
    size_t unique_bins_used; // unique bins used, used to compare against load_factor_capacity for resizing map
    size_t max_map_capacity; // maximum number of bins (resizable)

    float load_factor; // percentage of max_map_capacity to hit before resizing map
    size_t load_factor_capacity; // value of load_factor * max_map_capacity
};

// initialize new hasmap with initial capcacity, size of each key and size of each value (in bytes).
struct hashmap_t* map_init(
    size_t capacity, 
    size_t key_size, 
    size_t value_size, 
    size_t (*hash)(void*),
    int (*key_compare)(const void*, const void*));

// insert new key-value pair into the map.
// returns DUPLICATE_KEY (-1) if duplicate key is found, otherwise returns SUCCESS (0).
enum map_code_e map_put(struct hashmap_t** map, void* key, void* value);

// search a key in the map.
// if found, returns a pointer to the key_value pair, otherwise returns NULL
struct key_value_t* map_get(const struct hashmap_t* map, void* key);

// deallocate memory used in map
void map_free(struct hashmap_t** map);

#endif