cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INSTALL_PREFIX "./bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(stabix)

## Link directories
add_subdirectory(lib/pybind11)
add_subdirectory(lib/FastPFor)
add_subdirectory(lib/bxzstr)
if(NOT TARGET gmock)
        # can't be doubly adding.
        # Note, fastpfor depends on gtest
        add_subdirectory(lib/googletest)
endif()

## Include directories
include_directories(include)
include_directories(lib/FastPFor/headers)
#include_directories(lib/bzxstr/include)
#include_directories(lib/googletest/googletest/include)
include_directories(lib/pybind11/include/pybind11)

### Add the executables
# COMPRESSION
add_executable(gwas_compress
        src/compress_main.cpp
        src/blocks.cpp
        src/compress.cpp
        src/header.cpp
        src/utils.cpp
        src/cli/compress_cli.cpp)

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
        src/abstract_index/query_index.cpp
        src/cli/index_cli.cpp)

# DECOMPRESSION
add_executable(gwas_decompress
        src/decompress_main.cpp
        src/decompress.cpp
        src/header.cpp
        src/index.cpp
        src/utils.cpp
        src/abstract_index/indexers.cpp
        src/abstract_index/query_index.cpp
        src/abstract_index/build_index.cpp
        src/cli/decompress_cli.cpp)

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

target_link_libraries(gwas_compress bxzstr FastPFOR z)
target_link_libraries(gwas_index bxzstr FastPFOR z)
target_link_libraries(gwas_decompress bxzstr FastPFOR z)
#target_link_libraries(test_gwas gtest gtest_main bxzstr FastPFOR z)

# Regarding python bindings

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE ENTRYPOINT_SOURCES "src/cli/*.cpp")
list(REMOVE_ITEM SOURCES ${ENTRYPOINT_SOURCES})

SET(PYBIND11_PYTHON_VERSION 3.11.7)
set(PYTHON_EXECUTABLE "/opt/homebrew/bin/python3")

pybind11_add_module(stabixpy MODULE ${SOURCES})
target_link_libraries(stabixpy PRIVATE bxzstr FastPFOR z pybind11::module)
set_target_properties(stabixpy PROPERTIES PREFIX "" OUTPUT_NAME "stabix" SUFFIX ".so")