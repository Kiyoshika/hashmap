#include "hash_function.h"

size_t hash_int(void* key)
{
    size_t key_val = *(int*)key;
    key_val = ((key_val >> 16) ^ key_val) * 0x45d9f3b;
    key_val = ((key_val >> 16) ^ key_val) * 0x45d9f3b;
    key_val = (key_val >> 16) ^ key_val;

    return key_val;
}

size_t hash_float(void* key)
{
    float float_key = *(float*)key;
    float_key *= 7.0f;
    float_key -= 1.0f;
    float_key /= 13.0f;
    size_t rounded_key = roundf(float_key);

    return hash_int(&rounded_key);
}

size_t hash_double(void* key)
{
    double double_key = *(double*)key;
    double_key *= 7.0;
    double_key -= 1.0;
    double_key /= 13.0;
    size_t rounded_key = round(double_key);

    return hash_int(&rounded_key);
}

size_t hash_string(void* key)
{
    char* str_key = *(char**)key;
    size_t char_sum = 0;
    for (size_t i = 0; i < strlen(str_key); ++i)
        char_sum += str_key[i];
    
    return hash_int(char_sum);
}