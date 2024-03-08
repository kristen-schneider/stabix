cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 17)

project(gwas_run)

# Set the output folder where your program will be created
#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_BINARY_DIR bin/)

#find_package( ZLIB REQUIRED )

### Add the executables
# COMPRESSION
add_executable(gwas_compress
        src/compress_main.cpp
        src/utils.cpp
        src/header.cpp
        src/blocks.cpp
        src/compress.cpp
        src/decompress.cpp)

# INDEXING
add_executable(gwas_index
        src/index_main.cpp
        src/utils.cpp
        src/header.cpp
        src/decompress.cpp
        src/index.cpp)

# DECOMPRESSION
add_executable(gwas_decompress
        src/decompress_main.cpp
        src/header.cpp
        src/index.cpp
        src/utils.cpp
        src/decompress.cpp)

add_executable(test_gwas
        test/test_gwas.cpp
        test/test_compress.cpp
        test/test_decompress.cpp)

add_subdirectory(/Users/krsc0813/CLionProjects/gwas_local/lib/googletest-1.14.0/ googletest-1.14.0)

## Include directories
#target_include_directories(gwas_compress PRIVATE include)
#target_include_directories(gwas_index PRIVATE include)
#target_include_directories(gwas_decompress PRIVATE include)
include_directories(${ZLIB_INCLUDE_DIRS} include)
include_directories(lib/googletest-1.14.0)


# Additional configuration if needed, e.g., linking libraries, "adding -lz"
target_link_libraries(gwas_compress z)
target_link_libraries(gwas_index z)
target_link_libraries(gwas_decompress z)
target_link_libraries(test_gwas gtest gtest_main)

