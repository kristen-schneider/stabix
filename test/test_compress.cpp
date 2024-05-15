#include <iostream>

#include "compress.h"
#include "decompress.h"
#include "googletest/googletest/include/gtest/gtest.h"

using namespace std;

TEST(zlibCompress, simple) {
    string str = "a,b,c,d,e";
    string compressed_str = zlib_compress(str);
    string decompressed_str = zlib_decompress(compressed_str);
    ASSERT_EQ(decompressed_str, str);
}

TEST(zlibCompress, column){
    string col = "1,2,3,4,5";
    string compressed_str = zlib_compress(col);
    string decompressed_str = decompress_column(compressed_str,
                                                "zlib",
                                                0,
                                                5);
    ASSERT_EQ(decompressed_str, col);
}

TEST(fpfVB, small) {
    vector<uint32_t> col = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t block_size = 10;
    size_t compressedSize = 0; // Define compressedSize

    uint32_t* compressed_arr = fastpfor_vb_compress(col, compressedSize);
    vector<uint32_t> decompressed_col = fastpfor_vb_decompress(compressed_arr, compressedSize, block_size);

    for (int i = 0; i < col.size(); i++){
        ASSERT_EQ(col[i], decompressed_col[i]);
    }
}

TEST(fpfVB, medium){
    vector<uint32_t> col;
    size_t block_size = 100;
    // Create a vector of random integers
    for (int i = 0; i < block_size; i++){
        col.push_back(rand() % 1000);
    }
    size_t compressedSize = 0; // Define compressedSize

    uint32_t* compressed_arr = fastpfor_vb_compress(col, compressedSize);
    vector<uint32_t> decompressed_col = fastpfor_vb_decompress(compressed_arr, compressedSize, block_size);

    for (int i = 0; i < block_size; i++){
        ASSERT_EQ(col[i], decompressed_col[i]);
    }

}

TEST(fpfVB, large){
    vector<uint32_t> col;
    size_t block_size = 500;
    // Create a vector of random integers
    for (int i = 0; i < block_size; i++){
        col.push_back(rand() % 1000);
    }
    size_t compressedSize = 0; // Define compressedSize

    uint32_t* compressed_arr = fastpfor_vb_compress(col, compressedSize);
    vector<uint32_t> decompressed_col = fastpfor_vb_decompress(compressed_arr, compressedSize, block_size);

    for (int i = 0; i < block_size; i++){
        ASSERT_EQ(col[i], decompressed_col[i]);
    }
}

TEST(fpfVB, column){
    vector<uint32_t> col = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t block_size = 10;
    size_t compressedSize = 0; // Define compressedSize

    uint32_t* compressed_arr = fastpfor_vb_compress(col, compressedSize);
//    vector<uint32_t> decompressed_col = fastpfor_vb_decompress(compressed_arr, compressedSize, block_size);

    string decompressed_col = decompress_column((char*)compressed_arr,
                                                "fpfVB",
                                                compressedSize,
                                                block_size);
    ASSERT_EQ(decompressed_col, "0,1,2,3,4,5,6,7,8,9");
}