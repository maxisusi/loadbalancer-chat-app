#include "protocat.hpp"
#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_EQ(meaning_of_life(), 45);
  // EXPECT_STRNE("hello", "world");
  // // Expect equality.
  // EXPECT_EQ(7 * 6, 11);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
