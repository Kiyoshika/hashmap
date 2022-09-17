cmake_minimum_required(VERSION 3.10)
project(hashmap C)

# default build type = release mode
option(CMAKE_BUILD_TYPE Release)

set(CMAKE_C_FLAGS_DEBUG "-O0 -g -Wall")
set(CMAKE_C_FLAGS_RELEASE "-O2 -Wall")

add_subdirectory(src)