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
    string column_types_all_ints = get_data_types(all_ints, delimiter);
    string column_types_all_floats = get_data_types(all_floats, delimiter);
    string column_types_all_strings = get_data_types(all_strings, delimiter);
    string column_types_int_floats = get_data_types(int_floats, delimiter);
    string column_types_int_strings = get_data_types(int_strings, delimiter);
    string column_types_float_strings = get_data_types(float_strings, delimiter);

    ASSERT_EQ(column_types_all_ints, "int,int,int,int,int");
    ASSERT_EQ(column_types_all_floats, "float,float,float,float,float");
    ASSERT_EQ(column_types_all_strings, "string,string,string,string,string");
    ASSERT_EQ(column_types_int_floats, "int,float,int,float,int");
    ASSERT_EQ(column_types_int_strings, "int,string,int,string,int");
    ASSERT_EQ(column_types_float_strings, "float,string,float,string,float");
}

TEST(GetBlockBPbyMap, simmple){
    string map_file = "/Users/krsc0813/CLionProjects/gwas_local/map_files/test.map";
//    string map_file = "data/test.map";

    map<int, vector<uint32_t>> chrm_block_bp_ends = get_chrm_block_bp_ends(map_file);
    vector<uint32_t> chrm_vector = {5000, 9000, 13000};

    ASSERT_EQ(chrm_block_bp_ends.size(), 3);
    ASSERT_EQ(chrm_block_bp_ends[1], chrm_vector);
    ASSERT_EQ(chrm_block_bp_ends[1].size(), 3);
    ASSERT_EQ(chrm_block_bp_ends[2], chrm_vector);
    ASSERT_EQ(chrm_block_bp_ends[2].size(), 3);
    ASSERT_EQ(chrm_block_bp_ends[3], chrm_vector);
    ASSERT_EQ(chrm_block_bp_ends[3].size(), 3);
}

//// Data too large to include in git repo
//TEST(GetBlockBPbyMap, real){
//    string map_file = "/Users/krsc0813/CLionProjects/gwas_local/data/chrm_1-22.map";
//    map<int, vector<uint32_t>> chrm_block_bp_ends = get_chrm_block_bp_ends(map_file);
//
//    ASSERT_EQ(chrm_block_bp_ends.size(), 22);
//    ASSERT_EQ(chrm_block_bp_ends[1].size(), 286);
//    ASSERT_EQ(chrm_block_bp_ends[2].size(), 268);
//    ASSERT_EQ(chrm_block_bp_ends[3].size(), 223);
//    ASSERT_EQ(chrm_block_bp_ends[4].size(), 214);
//    ASSERT_EQ(chrm_block_bp_ends[5].size(), 204);
//    ASSERT_EQ(chrm_block_bp_ends[6].size(), 192);
//    ASSERT_EQ(chrm_block_bp_ends[7].size(), 187);
//    ASSERT_EQ(chrm_block_bp_ends[8].size(), 168);
//    ASSERT_EQ(chrm_block_bp_ends[9].size(), 166);
//    ASSERT_EQ(chrm_block_bp_ends[10].size(), 181);
//    ASSERT_EQ(chrm_block_bp_ends[11].size(), 158);
//    ASSERT_EQ(chrm_block_bp_ends[12].size(), 174);
//    ASSERT_EQ(chrm_block_bp_ends[13].size(), 125);
//    ASSERT_EQ(chrm_block_bp_ends[14].size(), 120);
//    ASSERT_EQ(chrm_block_bp_ends[15].size(), 141);
//    ASSERT_EQ(chrm_block_bp_ends[16].size(), 134);
//    ASSERT_EQ(chrm_block_bp_ends[17].size(), 128);
//    ASSERT_EQ(chrm_block_bp_ends[18].size(), 117);
//    ASSERT_EQ(chrm_block_bp_ends[19].size(), 107);
//    ASSERT_EQ(chrm_block_bp_ends[20].size(), 108);
//    ASSERT_EQ(chrm_block_bp_ends[21].size(), 62);
//    ASSERT_EQ(chrm_block_bp_ends[22].size(), 74);
//}
