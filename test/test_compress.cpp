#include <iostream>

#include "compress.h"
#include "decompress.h"
#include "gtest/gtest.h"

using namespace std;

TEST(zlibCompress, test1) {
    string str = "a,b,c,d,e";
    string compressed_str = zlib_compress(str);

    string decompressed_str = zlib_decompress(compressed_str);
    ASSERT_EQ(decompressed_str, str);
    //ASSERT_EQ(compressed_str, "x\xDAK\xD4I\xD2I\xD6I\xD1I\x5\0\r\x10\x2\xA0");
}

TEST(fastpforCompress, test1) {
    string str = "100";
    uint32_t* compressed_str = fastpfor_compress(str);

    string decompressed_str = fastpfor_decompress(compressed_str);
    ASSERT_EQ(decompressed_str, str);
}