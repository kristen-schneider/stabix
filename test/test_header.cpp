#include <vector>


#include "header.h"
#include "gtest/gtest.h"

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