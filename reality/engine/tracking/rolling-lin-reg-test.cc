// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":rolling-lin-reg",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x0c88c69c);

#include "reality/engine/tracking/rolling-lin-reg.h"

#include <gtest/gtest.h>

namespace c8 {

class RollingLinRegTest : public ::testing::Test {};

TEST_F(RollingLinRegTest, TestFit) {
  RollingLinReg linReg(6);
  linReg.slope(0, 0);
  EXPECT_FLOAT_EQ(linReg.slope(1, 0), 0.0f);
  EXPECT_FLOAT_EQ(linReg.slope(2, 1), 0.5f);
  EXPECT_FLOAT_EQ(linReg.slope(3, 1), 0.4f);
  EXPECT_FLOAT_EQ(linReg.slope(4, 2), 0.5f);
  EXPECT_FLOAT_EQ(linReg.slope(5, 2), 0.45714285f);
  EXPECT_FLOAT_EQ(linReg.slope(6, 1), 0.25714285);  // start poping!
  EXPECT_FLOAT_EQ(linReg.slope(7, 1), 0.0f);
}
}  // namespace c8
