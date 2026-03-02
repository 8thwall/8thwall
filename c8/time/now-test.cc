// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":now",
    "//c8:vector",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x7883ea6d);

#include "c8/vector.h"
#include "c8/time/now.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Eq;
using testing::Pointwise;

namespace c8 {
class NowTest : public ::testing::Test {};

TEST_F(NowTest, TimeReset) {
  TimeReset resetter;
  Vector<float> timeStamp {2.f, 3.f, 5.f};
  EXPECT_FLOAT_EQ(0.f, resetter.shift(timeStamp[0]));
  EXPECT_FLOAT_EQ(1.f, resetter.shift(timeStamp[1]));
  EXPECT_FLOAT_EQ(3.f, resetter.shift(timeStamp[2]));
}

TEST_F(NowTest, TimeResetInt64) {
  TimeReset resetter;
  Vector<int64_t> timeStamp {71770000000, 71770000001, 71770000003};
  EXPECT_FLOAT_EQ(0.f, resetter.shift(timeStamp[0]));
  EXPECT_FLOAT_EQ(1.f, resetter.shift(timeStamp[1]));
  EXPECT_FLOAT_EQ(3.f, resetter.shift(timeStamp[2]));
}

}
