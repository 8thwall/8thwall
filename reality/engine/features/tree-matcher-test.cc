// Copyright (c) 2024 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":tree-matcher",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xdc4a12bf);

#include "gtest/gtest.h"
#include "reality/engine/features/tree-matcher.h"

namespace c8 {

class TreeMatcherTest : public ::testing::Test {};

TEST_F(TreeMatcherTest, TestGetPopCountPattern) {
  TreeMatcher<ImageDescriptor<8>> matcher(2, 1, 64);
  ImageDescriptor<8> descriptor = {
    {0b00001000,
     0b011000000,
     0b10101010,
     0b11111111,
     0b00001100,
     0b011001000,
     0b11101010,
     0b00000100}};
  Vector<int> popCountPattern;
  matcher.getPopCountPattern(descriptor, &popCountPattern);
  Vector<int> expectedPopCountPattern = {3, 12, 5, 6};
  EXPECT_EQ(popCountPattern, expectedPopCountPattern);
}

}  // namespace c8