cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 17)

project(gwas_local)  # Replace "YourProjectName" with the actual name of your project

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

### Add the executable

## COMPRESSION
#add_executable(gwas_local src/compress_main.cpp
#        src/utils.cpp
#        src/header.cpp
#        src/blocks.cpp
#        src/compress.cpp
#        src/decompress.cpp)

## INDEXING
#add_executable(gwas_local src/index_main.cpp
#        src/utils.cpp
#        src/header.cpp
#        src/decompress.cpp
#        src/index.cpp)

# DECOMPRESSION
add_executable(gwas_local src/decompress_main.cpp
        src/header.cpp
        src/index.cpp
        src/utils.cpp
        src/decompress.cpp)

# Include directories
target_include_directories(gwas_local PRIVATE include)
#target_include_directories(gwas_local PRIVATE lib/FastPFor)

# Additional configuration if needed, e.g., linking libraries
# add -lz
target_link_libraries(gwas_local z)
