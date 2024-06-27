#include <iostream>

#include "compress.h"
#include "decompress.h"
#include "utils.h"
#include "gtest/gtest.h"

using namespace std;

// ------------------------
// TESTING ZLIB COMPRESSION
// ------------------------
TEST(zlibCompress, simpleChar) {
    // Test the zlib compression and decompression of a simple string of
    // characters
    string str = "a,b,c,d,e";
    string compressed_str = zlib_compress(str);
    string decompressed_str = zlib_decompress(compressed_str);
    ASSERT_EQ(decompressed_str, str);
}

TEST(zlibCompress, longChar) {
    // Test the zlib compression and decompression of a string of many
    // characters
    int num_chars = 1000;
    string str = "";
    for (int i = 0; i < num_chars; i++) {
        str += "a";
    }
    string compressed_str = zlib_compress(str);
    string decompressed_str = zlib_decompress(compressed_str);
    ASSERT_EQ(decompressed_str, str);
}

TEST(zlibCompress, simpleInt) {
    // Test the zlib compression and decompression of a simple string of
    // integers
    string str = "1,2,3,4,5";
    string compressed_str = zlib_compress(str);
    string decompressed_str = zlib_decompress(compressed_str);
    ASSERT_EQ(decompressed_str, str);
}

TEST(zlibCompress, longInt) {
    // Test the zlib compression and decompression of a string of many integers
    int num_ints = 1000;
    string str = "";
    for (int i = 0; i < num_ints; i++) {
        str += to_string(i);
        if (i != num_ints - 1) {
            str += ",";
        }
    }
    string compressed_str = zlib_compress(str);
    string decompressed_str = zlib_decompress(compressed_str);
    ASSERT_EQ(decompressed_str, str);
}

TEST(zlibCompress, columnChar) {
    // Test the zlib compression and decompression of a column of characters
    string col = "a,b,c,d,e";
    string compressed_str = zlib_compress(col);
    string decompressed_str = decompress_column(compressed_str, "zlib", 0, 5);
    ASSERT_EQ(decompressed_str, col);
}

TEST(zlibCompress, longColumnChar) {
    // Test the zlib compression and decompression of a column of characters
    int num_chars = 1000;
    string col = "";
    for (int i = 0; i < num_chars; i++) {
        col += "a";
    }
    string compressed_str = zlib_compress(col);
    string decompressed_str =
        decompress_column(compressed_str, "zlib", 0, num_chars);
    ASSERT_EQ(decompressed_str, col);
}

TEST(zlibCompress, columnInt) {
    // Test the zlib compression and decompression of a column of integers
    string col = "1,2,3,4,5";
    string compressed_str = zlib_compress(col);
    string decompressed_str = decompress_column(compressed_str, "zlib", 0, 5);
    ASSERT_EQ(decompressed_str, col);
}

TEST(zlibCompress, longColumnInt) {
    // Test the zlib compression and decompression of a column of integers
    int num_ints = 1000;
    string col = "";
    for (int i = 0; i < num_ints; i++) {
        col += to_string(i);
        if (i != num_ints - 1) {
            col += ",";
        }
    }
    string compressed_str = zlib_compress(col);
    string decompressed_str =
        decompress_column(compressed_str, "zlib", 0, num_ints);
    ASSERT_EQ(decompressed_str, col);
}

// -------------------------
// TESTING fpfVB COMPRESSION
// -------------------------
TEST(fpfVB, smallArray) {
    // Test the fastpfor_vb compression and decompression of a small array of
    // integers
    vector<uint32_t> col = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t block_size = 10;
    size_t compressed_size = 0; // Define compressedSize

    vector<uint32_t> compressed_arr =
        fastpfor_vb_compress(col, compressed_size);
    vector<uint32_t> decompressed_arr =
        fastpfor_vb_decompress(compressed_arr, compressed_size, block_size);

    for (int i = 0; i < col.size(); i++) {
        ASSERT_EQ(col[i], decompressed_arr[i]);
    }
}

TEST(fpfVB, mediumArray) {
    // Test the fastpfor_vb compression and decompression of a medium array of
    // integers
    vector<uint32_t> col;
    size_t block_size = 500;
    // Create a vector of random integers
    for (int i = 0; i < block_size; i++) {
        col.push_back(rand() % 1000);
    }
    size_t compressedSize = 0; // Define compressedSize

    vector<uint32_t> compressed_arr = fastpfor_vb_compress(col, compressedSize);
    vector<uint32_t> decompressedVec =
        fastpfor_vb_decompress(compressed_arr, compressedSize, block_size);

    for (int i = 0; i < block_size; i++) {
        ASSERT_EQ(col[i], decompressedVec[i]);
    }
}

TEST(fpfVB, largeArray) {
    // Test the fastpfor_vb compression and decompression of a large array of
    // integers
    vector<uint32_t> col;
    size_t block_size = 1000;
    // Create a vector of random integers
    for (int i = 0; i < block_size; i++) {
        col.push_back(rand() % 1000);
    }
    size_t compressedSize = 0; // Define compressedSize

    vector<uint32_t> compressed_arr = fastpfor_vb_compress(col, compressedSize);
    vector<uint32_t> decompressedVec =
        fastpfor_vb_decompress(compressed_arr, compressedSize, block_size);

    for (int i = 0; i < block_size; i++) {
        ASSERT_EQ(col[i], decompressedVec[i]);
    }
}

TEST(fpfVB, smallString) {
    // Test the fastpfor_vb compression and decompression including conversion
    // to string (write out)
    vector<uint32_t> col = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t block_size = 10;
    size_t compressedSize = 0; // Define compressedSize

    vector<uint32_t> compressed_arr = fastpfor_vb_compress(col, compressedSize);
    // convert to string mimicking writing out to compressed file
    string compressed_string =
        convert_vector_uint32_to_string(compressed_arr.data(), compressedSize);
    string decompressed_string = decompress_column(compressed_string, "fpfVB",
                                                   compressedSize, block_size);
    // convert back to vector of uint32_t
    vector<uint32_t> decompressed_arr =
        convert_string_to_vector_unsignedlong(decompressed_string);

    for (int i = 0; i < col.size(); i++) {
        ASSERT_EQ(col[i], decompressed_arr[i]);
    }
}

TEST(fpfVB, mediumString) {
    // Test the fastpfor_vb compression and decompression including conversion
    // to string (write out)
    vector<uint32_t> col;
    size_t block_size = 500;
    // Create a vector of random integers
    for (int i = 0; i < block_size; i++) {
        col.push_back(rand() % 1000);
    }
    size_t compressedSize = 0; // Define compressedSize

    vector<uint32_t> compressed_arr = fastpfor_vb_compress(col, compressedSize);
    // convert to string mimicking writing out to compressed file
    string compressed_string =
        convert_vector_uint32_to_string(compressed_arr.data(), compressedSize);
    string decompressed_string = decompress_column(compressed_string, "fpfVB",
                                                   compressedSize, block_size);
    // convert back to vector of uint32_t
    vector<uint32_t> decompressed_arr =
        convert_string_to_vector_unsignedlong(decompressed_string);

    for (int i = 0; i < block_size; i++) {
        ASSERT_EQ(col[i], decompressed_arr[i]);
    }
}

TEST(fpfVB, largeString) {
    // Test the fastpfor_vb compression and decompression of a large column of
    // integers
    vector<uint32_t> col;
    size_t block_size = 1000;
    // Create a vector of random integers
    for (int i = 0; i < block_size; i++) {
        col.push_back(rand() % 1000);
    }
    size_t compressedSize = 0; // Define compressedSize

    vector<uint32_t> compressed_arr = fastpfor_vb_compress(col, compressedSize);
    // convert to string mimicking writing out to compressed file
    string compressed_string =
        convert_vector_uint32_to_string(compressed_arr.data(), compressedSize);
    string decompressed_string = decompress_column(compressed_string, "fpfVB",
                                                   compressedSize, block_size);
    // convert back to vector of uint32_t
    vector<uint32_t> decompressed_arr =
        convert_string_to_vector_unsignedlong(decompressed_string);

    for (int i = 0; i < block_size; i++) {
        ASSERT_EQ(col[i], decompressed_arr[i]);
    }
}

// -------------------------
// TESTING libzippp COMPRESSION
// -------------------------

TEST(bxzstr, deflate_smallString) {
    const string input =
        "The (very) quick brown fox JUMPED over the 745 lazy dogs.";
    const string comp = deflate_compress(input);
    const string decomp = deflate_decompress(comp);
    ASSERT_EQ(input, decomp);
}

TEST(bxzstr, deflate_whitespace) {
    const string input = "   ";
    const string comp = deflate_compress(input);
    const string decomp = deflate_decompress(comp);
    ASSERT_EQ(input, decomp);
}

TEST(bxzstr, deflate_blank) {
    const string input = "";
    const string comp = deflate_compress(input);
    const string decomp = deflate_decompress(comp);
    ASSERT_EQ(input, decomp);
}

TEST(bxzstr, bz2) {
    const string input = "Many CoMpReSsIoN algorithms in a *single* lib.";
    const string comp = bz2_compress(input);
    const string decomp = bz2_decompress(comp);
    ASSERT_EQ(input, decomp);
}

TEST(bxzstr, xz) {
    const string input = "Perhaps r_a_n_d_o_m data would be more proper ";
    const string comp = xz_compress(input);
    const string decomp = xz_decompress(comp);
    ASSERT_EQ(input, decomp);
}

TEST(bxzstr, zstd) {
    const string input = "But, 1t may n0t m@k3 @ d1ff3r3nc3.";
    const string comp = zstd_compress(input);
    const string decomp = zstd_decompress(comp);
    ASSERT_EQ(input, decomp);
}
