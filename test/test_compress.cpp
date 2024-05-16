#include <iostream>

#include "compress.h"
#include "decompress.h"
#include "utils.h"
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

TEST(fpfVB, check_small_array) {
    vector<uint32_t> col = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t block_size = 10;
    size_t compressed_size = 0; // Define compressedSize

    uint32_t *compressed_arr = fastpfor_vb_compress(col, compressed_size);
    // print out the compressed array
    for (int i = 0; i < compressed_size; i++){
        uint32_t num = compressed_arr[i];
        // convert the number to string and back
        string num_str = to_string(num);
        uint32_t num_back = stoul(num_str);
        // check if the number is the same
        ASSERT_EQ(num, num_back);
    }
    vector<uint32_t> decompressed_col = fastpfor_vb_decompress(compressed_arr, compressed_size, block_size);

    for (int i = 0; i < col.size(); i++){
        ASSERT_EQ(col[i], decompressed_col[i]);
    }
}

TEST(fpfVB, check_bitstring) {
    vector<uint32_t> col = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t block_size = 10;
    size_t compressed_size = 0; // Define compressedSize

    uint32_t *compressed_arr = fastpfor_vb_compress(col, compressed_size);

}

TEST(fpfVB, medium){
    vector<uint32_t> col;
    size_t block_size = 100;
    // Create a vector of random integers
    for (int i = 0; i < block_size; i++){
        col.push_back(rand() % 1000);
    }
    size_t compressedSize = 0; // Define compressedSize

    uint32_t * compressed_arr = fastpfor_vb_compress(col, compressedSize);
    vector<uint32_t> decompressedVec = fastpfor_vb_decompress(compressed_arr, compressedSize, block_size);

    for (int i = 0; i < block_size; i++){
        ASSERT_EQ(col[i], decompressedVec[i]);
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

    uint32_t * compressed_arr = fastpfor_vb_compress(col, compressedSize);
    vector<uint32_t> decompressedVec = fastpfor_vb_decompress(compressed_arr, compressedSize, block_size);

    for (int i = 0; i < block_size; i++){
        ASSERT_EQ(col[i], decompressedVec[i]);
    }
}
//
//TEST(fpfVB, column){
//    vector<uint32_t> col = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//    size_t block_size = 10;
//    size_t compressedSize = 0; // Define compressedSize
//
//    vector<uint32_t> compressedVec = fastpfor_vb_compress(col, compressedSize);
//    string compressedStr = convert_vector_int_to_string(compressedVec);
//    string decompressed_col = decompress_column(compressedStr,
//                                                "fpfVB",
//                                                compressedSize,
//                                                block_size);
//    ASSERT_EQ(decompressed_col, "0,1,2,3,4,5,6,7,8,9");
//}