add_library(hashmap-util util.c)
target_include_directories(hashmap-util PUBLIC ${hashmap_SOURCE_DIR}/include)

add_library(hashmap hashmap.c hash_function.c)
target_include_directories(hashmap PUBLIC ${hashmap_SOURCE_DIR}/include)
target_link_libraries(hashmap hashmap-util m)