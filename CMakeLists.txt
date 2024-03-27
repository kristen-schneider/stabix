cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 17)

project(gwas_run)
#include(ExternalProject)
#include(FetchContent)

#find_package( ZLIB REQUIRED )

## Include directories
#include_directories(${ZLIB_INCLUDE_DIRS} include)
include_directories(include)
include_directories(lib/)
include_directories(lib/googletest-1.14.0)
include_directories(lib/FastPFor)
#include_directories(lib/FastPFor/headers)
#include_directories(${CMAKE_BINARY_DIR}/FastPFor/headers/)
#include_directories(lib/FastPFor/headers)
#include_directories(lib/FastPFor/src)
#include_directories(${INSTALL_DIR}/h)


## Link directories
add_subdirectory(/Users/krsc0813/CLionProjects/gwas_local/lib/googletest-1.14.0/ googletest-1.14.0)
add_subdirectory(/Users/krsc0813/CLionProjects/gwas_local/lib/FastPFor/ lib/FastPFor)

### add external libraries
#externalproject_add(FastPFor
#        SOURCE_DIR /Users/krsc0813/CLionProjects/gwas_local/lib/FastPFor
#        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/FastPFor
#)
#externalproject_add(FastPFor
#        GIT_REPOSITORY https://github.com/lemire/FastPFor
#        GIT_TAG master
#        PREFIX ${CMAKE_BINARY_DIR}/FastPFor
#        CONFIGURE_COMMAND cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/FastPFor
#        BUILD_COMMAND make
#        INSTALL_COMMAND make install
#        )
#        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/FastPFor
#        )

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
        src/header.cpp)

#ExternalProject_Get_Property(FastPFor INSTALL_DIR)
#ExternalProject_Get_Property(simde INSTALL_DIR)
#add_dependencies(gwas_compress FastPFor)

#target_include_directories(gwas_compress PRIVATE ${INSTALL_DIR}/include)

# Additional configuration if needed, e.g., linking libraries, "adding -lz"
target_link_libraries(gwas_compress z /Users/krsc0813/CLionProjects/gwas_local/lib/FastPFor/build/libFastPFOR.a)
#target_link_libraries(gwas_compress z)
#target_link_libraries(gwas_index z)
#target_link_libraries(gwas_decompress z)
#target_link_libraries(test_gwas gtest gtest_main z)



