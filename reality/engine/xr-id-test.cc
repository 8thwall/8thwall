// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":xr-id",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xe3f3e51d);

#include "gtest/gtest.h"
#include "reality/engine/xr-id.h"

namespace c8 {

class XrIdTest : public ::testing::Test {};

TEST_F(XrIdTest, TestAccessors) {
  XrId id(3, 5);
  EXPECT_EQ(id.index(), 3);
  EXPECT_EQ(id.id(), 5);
}

TEST_F(XrIdTest, TestOperators) {
  XrId id1(3, 5);
  XrId id2(3, 5);
  XrId id3 = id2;

  EXPECT_EQ(id1, id2);
  EXPECT_EQ(id1, id3);
}

}  // namespace c8
