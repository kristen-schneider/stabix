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