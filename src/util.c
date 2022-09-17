#include "util.h"

void map_err(const char* msg)
{
    fprintf(stderr, "%s\n", msg);
    abort();
}