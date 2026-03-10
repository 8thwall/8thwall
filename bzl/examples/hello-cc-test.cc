#include <gtest/gtest.h>

TEST(HelloTest, DoesNothingSpecial) {
  // Test that the world is sane using gtest.
  auto hello = []() { return "Hello, World!"; };
  EXPECT_EQ("Hello, World!", hello());
}
