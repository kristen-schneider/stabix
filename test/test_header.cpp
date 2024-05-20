#include <vector>


#include "header.h"
#include "compress.h"
#include "decompress.h"
#include "googletest/googletest/include/gtest/gtest.h"

using namespace std;

TEST(ParseHeaderList, NumColumns) {
    vector<string> header_list = {"3", "4", "col1", "col2", "col3", "1" , "2", "3", "4", "5", "6", "7" ,"8", "9", "10"};
    string num_columns_query = "num columns";
    string num_blocks_query = "num blocks";
    string column_names_query = "column names";
    string block_header_lengths_query = "block header end bytes";
    string block_lengths_query = "block end bytes";
    string block_sizes_query = "block sizes";

    vector<string> num_columns_query_list = parse_header_list(header_list, num_columns_query);
    vector<string> num_blocks_query_list = parse_header_list(header_list, num_blocks_query);
    vector<string> column_names_query_list = parse_header_list(header_list, column_names_query);
    vector<string> block_header_lengths_query_list = parse_header_list(header_list, block_header_lengths_query);
    vector<string> block_lengths_query_list = parse_header_list(header_list, block_lengths_query);
    vector<string> block_sizes_query_list = parse_header_list(header_list, block_sizes_query);

    ASSERT_EQ(num_columns_query_list[0], "3");
    ASSERT_EQ(num_blocks_query_list[0], "4");
    ASSERT_EQ(column_names_query_list[0], "col1");
    ASSERT_EQ(column_names_query_list[1], "col2");
    ASSERT_EQ(column_names_query_list[2], "col3");
    ASSERT_EQ(block_header_lengths_query_list[0], "1");
    ASSERT_EQ(block_header_lengths_query_list[1], "2");
    ASSERT_EQ(block_header_lengths_query_list[2], "3");
    ASSERT_EQ(block_header_lengths_query_list[3], "4");
    ASSERT_EQ(block_lengths_query_list[0], "5");
    ASSERT_EQ(block_lengths_query_list[1], "6");
    ASSERT_EQ(block_lengths_query_list[2], "7");
    ASSERT_EQ(block_lengths_query_list[3], "8");
    ASSERT_EQ(block_sizes_query_list[0], "9");
    ASSERT_EQ(block_sizes_query_list[1], "10");
}

TEST(RemoveHeader, simple){
    string text_1 = "hello world";
    string header_1 = "hello";
    string text_1_no_header = remove_zlib_header(text_1, header_1);
    ASSERT_EQ(text_1_no_header, " world");

    string text_2 = "goodbye, cruel world";
    string header_2 = "goodbye";
    string text_2_no_header = remove_zlib_header(text_2, header_2);
    ASSERT_EQ(text_2_no_header, ", cruel world");
}


TEST(RemoveZlibHeader, simple){
    string text_1 = "hello world";
    string text_1_compressed = zlib_compress(text_1);
    string text_1_decompressed = zlib_decompress(text_1_compressed);
    ASSERT_EQ(text_1, text_1_decompressed);


    string text_2 = "goodbye, cruel world";
    string text_2_compressed = zlib_compress(text_2);
    string text_2_decompressed = zlib_decompress(text_2_compressed);
    ASSERT_EQ(text_2, text_2_decompressed);

    string text_3 = "0,1,2,3,4,5,6,7,8,9";
    string text_3_compressed = zlib_compress(text_3);
    string text_3_decompressed = zlib_decompress(text_3_compressed);
    ASSERT_EQ(text_3, text_3_decompressed);
}