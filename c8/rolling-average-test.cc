// Copyright (c) 2024 Niantic, Inc.
// Original Author: Anvith Ekkati (anvith@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//c8:rolling-average",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x155162fb);

#include "c8/rolling-average.h"
#include "gmock/gmock.h"

namespace c8 {

class RollingAverageTest : public ::testing::Test {};

TEST_F(RollingAverageTest, EmptyWindow) {
  RollingAverage<int> rollingAverage(3);
  EXPECT_EQ(rollingAverage.average(), 0.);
}

TEST_F(RollingAverageTest, SingleValue) {
  RollingAverage<int> rollingAverage(3);
  rollingAverage.add(1);
  EXPECT_EQ(rollingAverage.average(), 1.);
}

TEST_F(RollingAverageTest, WindowNotFull) {
  RollingAverage<int> rollingAverage(3);
  rollingAverage.add(1);
  rollingAverage.add(2);
  EXPECT_EQ(rollingAverage.average(), 1.5);
}

TEST_F(RollingAverageTest, WindowFull) {
  RollingAverage<int> rollingAverage(3);
  rollingAverage.add(1);
  rollingAverage.add(2);
  rollingAverage.add(3);
  EXPECT_EQ(rollingAverage.average(), 2.);
}

TEST_F(RollingAverageTest, WindowOverflow) {
  RollingAverage<int> rollingAverage(3);
  rollingAverage.add(1);
  rollingAverage.add(2);
  rollingAverage.add(3);
  rollingAverage.add(4);
  EXPECT_EQ(rollingAverage.average(), 3.);
}

}  // namespace c8
