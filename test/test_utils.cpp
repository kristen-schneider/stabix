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

TEST(ConvertVectorToStringTest, CommaDelimiter) {
    vector<string> vec = {"a", "b", "c", "d", "e"};
    string str = convert_vector_to_string(vec);
    EXPECT_EQ("a,b,c,d,e", str);
}

TEST(ConvertVectorToStringTest, TabDelimiter) {
    vector<string> vec = {"a", "b", "c", "d", "e"};
    string str = convert_vector_to_string(vec);
    EXPECT_EQ("a,b,c,d,e", str);
}

TEST(ConvertVectorToStringTest, SpaceDelimiter) {
    vector<string> vec = {"a", "b", "c", "d", "e"};
    string str = convert_vector_to_string(vec);
    EXPECT_EQ("a,b,c,d,e", str);
}

TEST(ConvertStringToVectorTest, CommaDelimiter) {
    string str = "a,b,c,d,e";
    vector<string> vec = convert_string_to_vector(str);
    EXPECT_EQ(5, vec.size());
    EXPECT_EQ("a", vec[0]);
    EXPECT_EQ("b", vec[1]);
    EXPECT_EQ("c", vec[2]);
    EXPECT_EQ("d", vec[3]);
    EXPECT_EQ("e", vec[4]);
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