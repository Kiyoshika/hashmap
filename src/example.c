#include "hashmap.h"
#include <stdio.h>
#include <time.h>

/*
 * This example creates a hashmap with integer keys and integer values.
 * We create a dummy function to add a custom key-value pair into the map (this is
 * to specifically showcase you can add values to a map in different scopes) and
 * then we insert 100,000 random keys and values, then finally search for our custom
 * key-value pair we inserted to print to the console.
 */

struct key_t
{
    int key;
};

struct value_t
{
    int value;
};

void add(struct hashmap_t** map)
{
    struct key_t k = {9953};
    struct value_t v = {1002};
    map_put(map, &k, &v);
}

// comparison function is the same as what you'd use for qsort() or bsearch()
// A return value of 0 indicates the keys are equal
int key_compare(const void* a, const void* b)
{
    const struct key_t _a = *(const struct key_t*)a;
    const struct key_t _b = *(const struct key_t*)b;

    return _a.key - _b.key;
}

int main()
{
    struct hashmap_t* map = map_init(
        10, // initial capacity of 10
        sizeof(struct key_t), // size of our key
        sizeof(struct value_t), // size of our value
        &hash_int, // integer hash function
        &key_compare); // comparison function to determine when keys are equal (used in map_get)
    
    srand(time(NULL));

    add(&map);
    for (size_t i = 0; i < 100000; ++i)
    {
        int rand_key = rand() % 99999999;
        int rand_val = rand() % 99999999;
        map_put(&map, &(struct key_t){rand_key}, &(struct value_t){rand_val});
    }
    
    // fetch 100,000 random keys to demonstrate performance
    // since the rand_key is extremely large, it's very unlikely we will find many
    // keys, but this is just for demonstration purposes that we can search many keys
    // very quickly
    struct key_value_t* kv = NULL;
    size_t random_keys_found = 0;
    for (size_t i = 0; i < 100000; ++i)
    {
        // map_get returns a pointer to the key_value_t struct within the map containing
        // our key-value pair. DO NOT FREE THIS!!!!
        int rand_key = rand() % 99999999;
        kv = map_get(map, &(struct key_t){rand_key});
        if (kv)
            random_keys_found++;
    }
    printf("Found %zu / 100,000 random keys!\n", random_keys_found);
    

    // our custom key we inserted
    kv = map_get(map, &(struct key_t){9953});

    if (!kv)
        printf("Key not found!\n");
    else
        printf("Found Key: %d\nFound Value: %d\n", *(int*)kv->key, *(int*)kv->value);

    printf("Capacity: %zu\n", map->max_map_capacity);

    map_free(&map);

    return 0;
}