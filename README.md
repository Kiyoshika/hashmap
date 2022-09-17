# hashmap
This is a simple generic hashmap which supports user-defined types for both the keys and values (at the moment there is no support yet for custom deallocators if your custom types contain dynamically-allocated memory but this is planned soon).

The performance of this map is extremely quick, inserting and searching 100,000 keys almost instantaneously (on i7 processor, 32GB RAM) in debug mode. However, this map is very heap intensive with lots of allocations. The memory usage could probably be optimized more, but won't bother with that until it becomes a serious problem; didn't want to prematurely optimize.

There are built-in hash functions for the basic types (string, int, float, double) but you will have to provide your own hash function for user-defined types. See [hash functions](src/hash_function.c) for examples.

# Sample Usage
See [this](src/example.c) for a complete example of how the map is used.

# Building from Source
After cloning this repo, in the root directory you can:
1. `mkdir build && cd build`
2. `cmake ..` for Release mode (default) or
3. `cmake -DCMAKE_BUILD_TYPE=Debug` for debug mode
4. `make`

This will generate a static library `libhashmap.a`.

# Including in Other CMake Projects
You can add this project as a git submodule into your project. Just add the following in your CMakeLists wherever appropriate:
1. `add_subdirectory(/path/to/hashmap)`
2. `target_include_directories([your target] PUBLIC ${hashmap_SOURCE_DIR}/include)`
3. `target_link_libraries([your target] hashmap)

Then it should become a part of your build and you can use `#include "hashmap.h"`.
