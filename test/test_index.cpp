#include <iostream>
#include "gtest/gtest.h"

using namespace std;

int sub(int a, int b) {
    return a - b;
}

TEST(SubtractionTest, PositiveNumbers) {
    EXPECT_EQ(3, sub(5, 2));
    EXPECT_EQ(5, sub(8, 3));
    EXPECT_EQ(7, sub(11, 4));
}

TEST(SubtractionTest, NegativeNumbers) {
    EXPECT_EQ(-3, sub(-1, 2));
    EXPECT_EQ(-5, sub(-2, 3));
    EXPECT_EQ(-7, sub(-3, 4));
}

//
//int main(int argc, char **argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}

