#include <iostream>

#include "utils.h"
#include "gtest/gtest.h"

using namespace std;

int add(int a, int b) {
    return a + b;
}

TEST(SplitStringTest, CommaDelimiter) {
    string str = "a,b,c,d,e";
    char delimiter = ',';
    vector<string> vec = split_string(str, delimiter);
    EXPECT_EQ(5, vec.size());
    EXPECT_EQ("a", vec[0]);
    EXPECT_EQ("b", vec[1]);
    EXPECT_EQ("c", vec[2]);
    EXPECT_EQ("d", vec[3]);
    EXPECT_EQ("e", vec[4]);
}

TEST(SplitStringTest, TabDelimiter) {
    string str = "a\tb\tc\td\te";
    char delimiter = '\t';
    vector<string> vec = split_string(str, delimiter);
    EXPECT_EQ(5, vec.size());
    EXPECT_EQ("a", vec[0]);
    EXPECT_EQ("b", vec[1]);
    EXPECT_EQ("c", vec[2]);
    EXPECT_EQ("d", vec[3]);
    EXPECT_EQ("e", vec[4]);
}

TEST(SplitStringTest, SpaceDelimiter) {
    string str = "a b c d e";
    char delimiter = ' ';
    vector<string> vec = split_string(str, delimiter);
    EXPECT_EQ(5, vec.size());
    EXPECT_EQ("a", vec[0]);
    EXPECT_EQ("b", vec[1]);
    EXPECT_EQ("c", vec[2]);
    EXPECT_EQ("d", vec[3]);
    EXPECT_EQ("e", vec[4]);
}

TEST(ConvertVectorStrToStringTest, CommaDelimiter) {
    vector<string> vec = {"a", "b", "c", "d", "e"};
    string str = convert_vector_str_to_string(vec);
    EXPECT_EQ("a,b,c,d,e", str);
}

TEST(ConvertVectorStrToStringTest, TabDelimiter) {
    vector<string> vec = {"a", "b", "c", "d", "e"};
    string str = convert_vector_str_to_string(vec);
    EXPECT_EQ("a,b,c,d,e", str);
}

TEST(ConvertVectorStrToStringTest, SpaceDelimiter) {
    vector<string> vec = {"a", "b", "c", "d", "e"};
    string str = convert_vector_str_to_string(vec);
    EXPECT_EQ("a,b,c,d,e", str);
}

TEST(ConvertVectorIntToStringTest, PositiveNumbers) {
    vector<int> vec = {1, 2, 3, 4, 5};
    string str = convert_vector_int_to_string(vec);
    EXPECT_EQ("1,2,3,4,5", str);
}

TEST(ConvertStringToVectorTest, CommaDelimiter) {
    string str = "a,b,c,d,e";
    vector<string> vec = convert_string_to_vector_string(str);
    EXPECT_EQ(5, vec.size());
    EXPECT_EQ("a", vec[0]);
    EXPECT_EQ("b", vec[1]);
    EXPECT_EQ("c", vec[2]);
    EXPECT_EQ("d", vec[3]);
    EXPECT_EQ("e", vec[4]);
}

TEST(convertStringToUint32_t, CommaDelimiter) {
    string str = "2206368128,2273740164,35208";
    vector<uint32_t> expected_vector = {2206368128, 2273740164, 35208};
    vector<uint32_t> vec = convert_string_to_vector_unsignedlong(str);
    EXPECT_EQ(expected_vector.size(), vec.size());
    for (int i = 0; i < expected_vector.size(); i++) {
        EXPECT_EQ(expected_vector[i], vec[i]);
    }
}

TEST(convertUint32_tToString, CommaDelimiter){
    size_t compressed_size = 3;
    uint32_t * compressed_arrr = new uint32_t[compressed_size];
    compressed_arrr[0] = 2206368128;
    compressed_arrr[1] = 2273740164;
    compressed_arrr[2] = 35208;
    string expected_string = "2206368128,2273740164,35208";
    string str = convert_vector_uint32_to_string(compressed_arrr, compressed_size);
    EXPECT_EQ(expected_string, str);
}

TEST(convertStringToVectorUint, CmommaDelimiter){
    string str = "2206368128,2273740164,35208";
    uint32_t * expected_vector = new uint32_t[3];
    expected_vector[0] = 2206368128;
    expected_vector[1] = 2273740164;
    expected_vector[2] = 35208;
    uint32_t * vec = convert_string_to_vector_uint32(str, ',');
    for (int i = 0; i < 3; i++){
        EXPECT_EQ(expected_vector[i], vec[i]);
    }
}

TEST(IntToBytesTest, PositiveNumber) {
    int value = 123456789;
    char * bytes = int_to_bytes(value);
    int result = bytes_to_int(bytes);
    EXPECT_EQ(value, result);
}

TEST(IntToBytesTest, NegativeNumber) {
    int value = -123456789;
    char * bytes = int_to_bytes(value);
    int result = bytes_to_int(bytes);
    EXPECT_EQ(value, result);
}

TEST(BytesToIntTest, PositiveNumber) {
    int value = 123456789;
    char * bytes = int_to_bytes(value);
    int result = bytes_to_int(bytes);
    EXPECT_EQ(value, result);
}

TEST(BytesToIntTest, NegativeNumber) {
    int value = -123456789;
    char * bytes = int_to_bytes(value);
    int result = bytes_to_int(bytes);
    EXPECT_EQ(value, result);
}

TEST(GetIndexTest, PostiveTest) {
    vector<string> vec = {"a", "b", "c", "d", "e"};
    string str_a = "a";
    string str_b = "b";
    string str_c = "c";
    string str_d = "d";
    string str_e = "e";
    int idx_a = get_index(vec, str_a);
    int idx_b = get_index(vec, str_b);
    int idx_c = get_index(vec, str_c);
    int idx_d = get_index(vec, str_d);
    int idx_e = get_index(vec, str_e);
    EXPECT_EQ(0, idx_a);
    EXPECT_NE(-1, idx_a);
    EXPECT_EQ(1, idx_b);
    EXPECT_NE(-1, idx_b);
    EXPECT_EQ(2, idx_c);
    EXPECT_NE(-1, idx_c);
    EXPECT_EQ(3, idx_d);
    EXPECT_NE(-1, idx_d);
    EXPECT_EQ(4, idx_e);
    EXPECT_NE(-1, idx_e);
}

TEST(GetIndexTest, NegativeTest) {
    vector<string> vec = {"a", "b", "c", "d", "e"};
    string str_a = "a";
    string str_f = "f";
    int idx_a = get_index(vec, str_a);
    int idx_f = get_index(vec, str_f);
    EXPECT_NE(1, idx_a);
    EXPECT_EQ(-1, idx_f);
}

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

TEST(GetColumnTypes, Comma) {
    string all_ints = "1,2,3,4,5";
    string all_floats = "1.1,2.2,3.3,4.4,5.5";
    string all_strings = "a,b,c,d,e";
    string int_floats = "1,2.2,3,4.4,5";
    string int_strings = "1,a,3,b,5";
    string float_strings = "1.1,a,3.3,b,5.5";

    char delimiter = ',';
    vector<string> column_types_all_ints = get_data_types(all_ints, delimiter);
    vector<string> column_types_all_floats = get_data_types(all_floats, delimiter);
    vector<string> column_types_all_strings = get_data_types(all_strings, delimiter);
    vector<string> column_types_int_floats = get_data_types(int_floats, delimiter);
    vector<string> column_types_int_strings = get_data_types(int_strings, delimiter);
    vector<string> column_types_float_strings = get_data_types(float_strings, delimiter);

    for (int i = 0; i < 5; i++) {
        ASSERT_EQ("int", column_types_all_ints[i]);
        ASSERT_EQ("float", column_types_all_floats[i]);
        ASSERT_EQ("string", column_types_all_strings[i]);
    }

    ASSERT_EQ("int", column_types_int_floats[0]);
    ASSERT_EQ("float", column_types_int_floats[1]);
    ASSERT_EQ("int", column_types_int_floats[2]);
    ASSERT_EQ("float", column_types_int_floats[3]);
    ASSERT_EQ("int", column_types_int_floats[4]);

    ASSERT_EQ("int", column_types_int_strings[0]);
    ASSERT_EQ("string", column_types_int_strings[1]);
    ASSERT_EQ("int", column_types_int_strings[2]);
    ASSERT_EQ("string", column_types_int_strings[3]);
    ASSERT_EQ("int", column_types_int_strings[4]);

    ASSERT_EQ("float", column_types_float_strings[0]);
    ASSERT_EQ("string", column_types_float_strings[1]);
    ASSERT_EQ("float", column_types_float_strings[2]);
    ASSERT_EQ("string", column_types_float_strings[3]);
    ASSERT_EQ("float", column_types_float_strings[4]);
}

TEST(GetBlockBPbyMap, simmple){
    string map_file = "/Users/krsc0813/CLionProjects/gwas_local/map_files/test.map";
//    string map_file = "data/test.map";

    map<int, vector<uint32_t>> chrm_block_bp_ends = read_cm_map_file(map_file);
    vector<uint32_t> chrm_vector = {5000, 9000, 13000};

    ASSERT_EQ(chrm_block_bp_ends.size(), 3);
    ASSERT_EQ(chrm_block_bp_ends[1], chrm_vector);
    ASSERT_EQ(chrm_block_bp_ends[1].size(), 3);
    ASSERT_EQ(chrm_block_bp_ends[2], chrm_vector);
    ASSERT_EQ(chrm_block_bp_ends[2].size(), 3);
    ASSERT_EQ(chrm_block_bp_ends[3], chrm_vector);
    ASSERT_EQ(chrm_block_bp_ends[3].size(), 3);
}

TEST(GetBlockIDbyLineID, simple){
    // make a map of lineID to blockID
    map<int, int> lineID_blockID_map;
    lineID_blockID_map[0] = 0;
    lineID_blockID_map[1] = 0;
    lineID_blockID_map[2] = 0;
    lineID_blockID_map[3] = 0;
    lineID_blockID_map[4] = 0;
    lineID_blockID_map[5] = 1;
    lineID_blockID_map[6] = 1;
    lineID_blockID_map[7] = 1;
    lineID_blockID_map[8] = 1;
    lineID_blockID_map[9] = 1;
    lineID_blockID_map[10] = 2;
    lineID_blockID_map[11] = 2;
    lineID_blockID_map[12] = 2;
    lineID_blockID_map[13] = 2;
    lineID_blockID_map[14] = 2;

    auto mapper = BlockLineMap(lineID_blockID_map);

    // for 0-14, the blockID should be 0, 1, 2
    for (int i = 0; i < 15; i++){
        ASSERT_EQ(mapper.line_to_block(i), lineID_blockID_map[i]);
    }
}


TEST(GetBlockIDbyLineID, line_between_blocks){
    map<int, int> lineID_blockID_map;
    lineID_blockID_map[0] = 0;
    lineID_blockID_map[5] = 1;
    lineID_blockID_map[10] = 2;
    auto mapper = BlockLineMap(lineID_blockID_map);

    // for 0-4, the blockID should be 0
    for (int i = 0; i < 5; i++){
        ASSERT_EQ(mapper.line_to_block(i), 0);
    }
    // for 5-9, the blockID should be 1
    for (int i = 5; i < 10; i++){
        ASSERT_EQ(mapper.line_to_block(i), 1);
    }
    // for 10-14, the blockID should be 2
    for (int i = 10; i < 15; i++){
        ASSERT_EQ(mapper.line_to_block(i), 2);
    }
}