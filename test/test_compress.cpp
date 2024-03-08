#include <iostream>
#include "gtest/gtest.h"

using namespace std;

int add(int a, int b) {
    return a + b;
}

TEST(AdditionTest, PositiveNumbers) {
    EXPECT_EQ(3, add(1, 2));
    EXPECT_EQ(5, add(2, 3));
    EXPECT_EQ(7, add(3, 4));
}

TEST(AdditionTest, NegativeNumbers) {
    EXPECT_EQ(-3, add(-1, -2));
    EXPECT_EQ(-5, add(-2, -3));
    EXPECT_EQ(-7, add(-3, -4));
}
//
//int main(int argc, char **argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}
//
//
