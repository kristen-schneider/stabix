#include <vector>


#include "header.h"
#include "gtest/gtest.h"

using namespace std;

TEST(GetDelimiter, Comma) {
    string line1 = "a,b,c,d,e";
    string line2 = "apple,banana,carrot,dog,elephant";
    string line3 = "1,2,3,4,5";
    char delimiter1 = get_delimiter(line1);
    char delimiter2 = get_delimiter(line2);
    char delimiter3 = get_delimiter(line3);
    ASSERT_EQ(delimiter1, ',');
    ASSERT_EQ(delimiter2, ',');
    ASSERT_EQ(delimiter3, ',');
}

TEST(GetDelimiter, Tab) {
    string line1 = "a\tb\tc\td\te";
    string line2 = "apple\tbanana\tcarrot\tdog\telephant";
    string line3 = "1\t2\t3\t4\t5";
    char delimiter1 = get_delimiter(line1);
    char delimiter2 = get_delimiter(line2);
    char delimiter3 = get_delimiter(line3);
    ASSERT_EQ(delimiter1, '\t');
    ASSERT_EQ(delimiter2, '\t');
    ASSERT_EQ(delimiter3, '\t');
}

TEST(GetDelimiter, Space) {
    string line1 = "a b c d e";
    string line2 = "apple banana carrot dog elephant";
    string line3 = "1 2 3 4 5";
    char delimiter1 = get_delimiter(line1);
    char delimiter2 = get_delimiter(line2);
    char delimiter3 = get_delimiter(line3);
    ASSERT_EQ(delimiter1, ' ');
    ASSERT_EQ(delimiter2, ' ');
    ASSERT_EQ(delimiter3, ' ');
}

TEST(GetColumnNames, Comma) {
    string line = "a,b,c,d,e";
    char delimiter = ',';
    string column_names = get_column_names(line, delimiter);
    ASSERT_EQ(column_names, "a,b,c,d,e");
}

TEST(GetColumnNames, Tab) {
    string line = "a\tb\tc\td\te";
    char delimiter = '\t';
    string column_names = get_column_names(line, delimiter);
    ASSERT_EQ(column_names, "a,b,c,d,e");
}

TEST(GetColumnNames, Space) {
    string line = "a b c d e";
    char delimiter = ' ';
    string column_names = get_column_names(line, delimiter);
    ASSERT_EQ(column_names, "a,b,c,d,e");
}

TEST(GetColumnTypes, Comma) {
    string all_ints = "1,2,3,4,5";
    string all_floats = "1.1,2.2,3.3,4.4,5.5";
    string all_strings = "a,b,c,d,e";
    string int_floats = "1,2.2,3,4.4,5";
    string int_strings = "1,a,3,b,5";
    string float_strings = "1.1,a,3.3,b,5.5";

    char delimiter = ',';
    string column_types_all_ints = get_column_types(all_ints, delimiter);
    string column_types_all_floats = get_column_types(all_floats, delimiter);
    string column_types_all_strings = get_column_types(all_strings, delimiter);
    string column_types_int_floats = get_column_types(int_floats, delimiter);
    string column_types_int_strings = get_column_types(int_strings, delimiter);
    string column_types_float_strings = get_column_types(float_strings, delimiter);

    ASSERT_EQ(column_types_all_ints, "int,int,int,int,int");
    ASSERT_EQ(column_types_all_floats, "float,float,float,float,float");
    ASSERT_EQ(column_types_all_strings, "string,string,string,string,string");
    ASSERT_EQ(column_types_int_floats, "int,float,int,float,int");
    ASSERT_EQ(column_types_int_strings, "int,string,int,string,int");
    ASSERT_EQ(column_types_float_strings, "float,string,float,string,float");
}

TEST(ParseHeaderList, NumColumns) {
    vector<string> header_list = {"3", "4", "col1", "col2", "col3", "1" , "2", "3", "4", "5", "6", "7" ,"8", "9", "10"};
    string num_columns_query = "num columns";
    string num_blocks_query = "num blocks";
    string column_names_query = "column names";
    string block_header_lengths_query = "block header lengths";
    string block_lengths_query = "block lengths";
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