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