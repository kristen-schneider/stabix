cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(gwas_run)

## Include directories
#include_directories(${ZLIB_INCLUDE_DIRS} include)
include_directories(include)
include_directories(lib)
include_directories(lib/googletest/googletest/include)
#include_directories(lib/FastPFor)

## Link directories
add_subdirectory(lib/googletest)
#add_subdirectory(lib/FastPFor)

### Add the executables
# COMPRESSION
add_executable(gwas_compress
        src/compress_main.cpp
        src/blocks.cpp
        src/compress.cpp
        src/header.cpp
        src/utils.cpp)

# INDEXING
add_executable(gwas_index
        src/index_main.cpp
        src/decompress.cpp
        src/header.cpp
        src/index.cpp
        src/utils.cpp)

# DECOMPRESSION
add_executable(gwas_decompress
        src/decompress_main.cpp
        src/decompress.cpp
        src/header.cpp
        src/index.cpp
        src/utils.cpp)

add_executable(test_gwas
        test/test_gwas.cpp
        test/test_utils.cpp
        src/utils.cpp
        test/test_index.cpp
        src/index.cpp
        src/decompress.cpp
        test/test_header.cpp
        src/header.cpp
        test/test_compress.cpp
        src/compress.cpp)

# Additional configuration if needed, e.g., linking libraries, "adding -lz"
target_link_libraries(gwas_compress z)
#target_link_libraries(gwas_index z)
#target_link_libraries(gwas_decompress z)
target_link_libraries(test_gwas gtest gtest_main z)



