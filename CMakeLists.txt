cmake_minimum_required(VERSION 3.27)

set(CMAKE_PREFIX_PATH "/Users/krsc0813/miniforge3/envs/test")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INSTALL_PREFIX "./bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(gwas_run)

## Link directories
add_subdirectory(lib/FastPFor)
add_subdirectory(lib/bxzstr)
if(NOT TARGET gmock)
        # can't be doubly adding.
        # Note, fastpfor depends on gtest
        add_subdirectory(lib/googletest)
endif()

## Include directories
include_directories(include)
include_directories(lib)
include_directories(lib/FastPFor/headers)
include_directories(lib/bzxstr/include)
include_directories(lib/googletest/googletest/include)

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
        src/abstract_index/indexers.cpp
        src/decompress.cpp
        src/header.cpp
        src/index.cpp
        src/utils.cpp
        src/abstract_index/indexers.cpp
        src/abstract_index/build_index.cpp
        src/abstract_index/query_index.cpp)

# DECOMPRESSION
add_executable(gwas_decompress
        src/decompress_main.cpp
        src/decompress.cpp
        src/header.cpp
        src/index.cpp
        src/utils.cpp
        src/abstract_index/indexers.cpp
        src/abstract_index/query_index.cpp
        src/abstract_index/build_index.cpp)

## TESTING
#add_executable(test_gwas
#        test/test_gwas.cpp
#        test/test_utils.cpp
#        src/utils.cpp
#        test/test_index.cpp
#        src/index.cpp
#        src/decompress.cpp
#        test/test_header.cpp
#        src/header.cpp
#        test/test_compress.cpp
#        src/compress.cpp
#        src/blocks.cpp
#        test/test_blocks.cpp)

# Additional configuration if needed, e.g., linking libraries, "adding -lz"
#target_link_libraries(gwas_compress bxzstr z)
#target_link_libraries(gwas_index bxzstr z)
#target_link_libraries(gwas_decompress bxzstr z)
#target_link_libraries(test_gwas gtest gtest_main bxzstr z)

target_link_libraries(gwas_compress bxzstr FastPFOR z)
target_link_libraries(gwas_index bxzstr FastPFOR z)
target_link_libraries(gwas_decompress bxzstr FastPFOR z)
#target_link_libraries(test_gwas gtest gtest_main bxzstr FastPFOR z)
