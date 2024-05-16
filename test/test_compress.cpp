#include <iostream>

#include "compress.h"
#include "decompress.h"
#include "utils.h"
#include "googletest/googletest/include/gtest/gtest.h"

using namespace std;

// ------------------------
// TESTING ZLIB COMPRESSION
// ------------------------
TEST(zlibCompress, simpleChar) {
    // Test the zlib compression and decompression of a simple string of characters
    string str = "a,b,c,d,e";
    string compressed_str = zlib_compress(str);
    string decompressed_str = zlib_decompress(compressed_str);
    ASSERT_EQ(decompressed_str, str);
}

TEST(zlibCompress, longChar) {
    // Test the zlib compression and decompression of a string of many characters
    int num_chars = 1000;
    string str = "";
    for (int i = 0; i < num_chars; i++){
        str += "a";
    }
    string compressed_str = zlib_compress(str);
    string decompressed_str = zlib_decompress(compressed_str);
    ASSERT_EQ(decompressed_str, str);
}

TEST(zlibCompress, simpleInt) {
    // Test the zlib compression and decompression of a simple string of integers
    string str = "1,2,3,4,5";
    string compressed_str = zlib_compress(str);
    string decompressed_str = zlib_decompress(compressed_str);
    ASSERT_EQ(decompressed_str, str);
}

TEST(zlibCompress, longInt) {
    // Test the zlib compression and decompression of a string of many integers
    int num_ints = 1000;
    string str = "";
    for (int i = 0; i < num_ints; i++){
        str += to_string(i);
        if (i != num_ints - 1){
            str += ",";
        }
    }
    string compressed_str = zlib_compress(str);
    string decompressed_str = zlib_decompress(compressed_str);
    ASSERT_EQ(decompressed_str, str);
}

TEST(zlibCompress, columnChar){
    // Test the zlib compression and decompression of a column of characters
    string col = "a,b,c,d,e";
    string compressed_str = zlib_compress(col);
    string decompressed_str = decompress_column(compressed_str,
                                                "zlib",
                                                0,
                                                5);
    ASSERT_EQ(decompressed_str, col);
}

TEST(zlibCompress, longColumnChar){
    // Test the zlib compression and decompression of a column of characters
    int num_chars = 1000;
    string col = "";
    for (int i = 0; i < num_chars; i++){
        col += "a";
    }
    string compressed_str = zlib_compress(col);
    string decompressed_str = decompress_column(compressed_str,
                                                "zlib",
                                                0,
                                                num_chars);
    ASSERT_EQ(decompressed_str, col);
}

TEST(zlibCompress, columnInt){
    // Test the zlib compression and decompression of a column of integers
    string col = "1,2,3,4,5";
    string compressed_str = zlib_compress(col);
    string decompressed_str = decompress_column(compressed_str,
                                                "zlib",
                                                0,
                                                5);
    ASSERT_EQ(decompressed_str, col);
}

TEST(zlibCompress, longColumnInt){
    // Test the zlib compression and decompression of a column of integers
    int num_ints = 1000;
    string col = "";
    for (int i = 0; i < num_ints; i++){
        col += to_string(i);
        if (i != num_ints - 1){
            col += ",";
        }
    }
    string compressed_str = zlib_compress(col);
    string decompressed_str = decompress_column(compressed_str,
                                                "zlib",
                                                0,
                                                num_ints);
    ASSERT_EQ(decompressed_str, col);
}

// -------------------------
// TESTING fpfVB COMPRESSION
// -------------------------
TEST(fpfVB, smallArray) {
    // Test the fastpfor_vb compression and decompression of a small array of integers
    vector<uint32_t> col = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t block_size = 10;
    size_t compressed_size = 0; // Define compressedSize

    uint32_t *compressed_arr = fastpfor_vb_compress(col, compressed_size);
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

TEST(fpfVB, mediumArray){
    // Test the fastpfor_vb compression and decompression of a medium array of integers
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

TEST(fpfVB, largeArray){
    // Test the fastpfor_vb compression and decompression of a large array of integers
    vector<uint32_t> col;
    size_t block_size = 1000;
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

TEST(fpfVB, smallColumn){
    // Test the fastpfor_vb compression and decompression of a small column of integers
    vector<uint32_t> col = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t block_size = 10;
    size_t compressedSize = 0; // Define compressedSize

    uint32_t * compressed_arr = fastpfor_vb_compress(col, compressedSize);
    vector<uint32_t> decompressedVec = fastpfor_vb_decompress(compressed_arr, compressedSize, block_size);

    for (int i = 0; i < col.size(); i++){
        ASSERT_EQ(col[i], decompressedVec[i]);
    }
}

TEST(fpfVB, mediumColumn){
    // Test the fastpfor_vb compression and decompression of a medium sized column of integers
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

TEST(fpfVB, largeColumn){
    // Test the fastpfor_vb compression and decompression of a large column of integers
    vector<uint32_t> col;
    size_t block_size = 1000;
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