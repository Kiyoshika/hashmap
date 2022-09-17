#ifndef HASH_FUNCTION_H
#define HASH_FUNCTION_H

#include <stddef.h>

// integer hash function taken from https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
size_t hash_int(void* key);

size_t hash_float(void* key);

size_t hash_double(void* key);

size_t hash_string(void* key);

#endif