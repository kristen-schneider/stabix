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
    string decompressed_str = decompress_column(compressed_str, "zlib");
    ASSERT_EQ(decompressed_str, col);
}

TEST(fpfVB, simple) {
    vector<uint32_t> col = {1, 2, 3, 4, 5};
    uint32_t* compressed_str = fastpfor_vb_compress(col);
    vector<uint32_t> decompressed_col = fastpfor_vb_decompress(compressed_str);

    for (int i = 0; i < col.size(); i++){
        ASSERT_EQ(col[i], decompressed_col[i]);
    }
    for (int i = 0; i < decompressed_col.size(); i++){
        cout << col[i] << " " << decompressed_col[i] << endl;
        ASSERT_EQ(col[i], decompressed_col[i]);
    }

    int x;
}

//TEST(fpfVB, column){
//    vector<uint32_t> col = {1, 2, 3, 4, 5};
//    uint32_t* compressed_str = fastpfor_vb_compress(col);
//    string decompressed_str = decompress_column((char*)compressed_str, "fpfVB");
//    ASSERT_EQ(decompressed_str, "1,2,3,4,5");
//}