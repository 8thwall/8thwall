// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":feature-scales", "//bzl/inliner:rules", "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xdc4110a4);

#include "reality/engine/features/feature-scales.h"

#include "gtest/gtest.h"

namespace c8 {

class FeatureScalesTest : public ::testing::Test {};

/*
For Scale 0 seen at distance 5, dmin is 1.2738276131297606, dmax is 5.477225575051661
+ Dist at scale  0: (4.56, 5.00, 5.48]  1: (3.80, 4.17, 4.56]  2: (3.17, 3.47, 3.80]  3:
(2.64, 2.89, 3.17]  4: (2.20, 2.41, 2.64]  5: (1.83, 2.01, 2.20]  6: (1.53, 1.67, 1.83]  7:
(1.27, 1.40, 1.53] -1 0 0 0 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 6 6 6 7 7 7 -1

For Scale 5 seen at distance 5, dmin is 3.169690726303045, dmax is 13.629089942912547
+ Dist at scale  0: (11.36, 12.44, 13.63]  1: (9.46, 10.37, 11.36]  2: (7.89, 8.64, 9.46]  3:
(6.57, 7.20, 7.89]  4: (5.48, 6.00, 6.57]  5: (4.56, 5.00, 5.48]  6: (3.80, 4.17, 4.56]  7:
(3.17, 3.47, 3.80] -1 0 0 0 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 6 6 6 7 7 7 -1

For Scale 7 seen at distance 5, dmin is 4.564354645876384, dmax is 19.625889517794064
+ Dist at scale  0: (16.35, 17.92, 19.63]  1: (13.63, 14.93, 16.35]  2: (11.36, 12.44, 13.63]  3:
(9.46, 10.37, 11.36]  4: (7.89, 8.64, 9.46]  5: (6.57, 7.20, 7.89]  6: (5.48, 6.00, 6.57]  7:
(4.56, 5.00, 5.48] -1 0 0 0 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 6 6 6 7 7 7 -1
*/
TEST_F(FeatureScalesTest, DMinForScale) {
  EXPECT_FLOAT_EQ(dMinForScale(0, 5.0f), 1.3954082f); //1.2738276131297606f);
  // EXPECT_FLOAT_EQ(dMinForScale(5, 5.0f), 3.169690726303045f);
  // EXPECT_FLOAT_EQ(dMinForScale(7, 5.0f), 4.564354645876384f);
}

TEST_F(FeatureScalesTest, DMaxForDMin) {
  EXPECT_FLOAT_EQ(dMaxForDMin(1.2738276131297606f), 5.477225575051661f);
  EXPECT_FLOAT_EQ(dMaxForDMin(3.169690726303045f), 13.629089942912547f);
  EXPECT_FLOAT_EQ(dMaxForDMin(4.564354645876384f), 19.625889517794064f);
  EXPECT_FLOAT_EQ(dMaxForDMin(-1.0f), FLT_MAX);
}

TEST_F(FeatureScalesTest, ScaleForDist) {
  float dmn = dMinForScale(0, 5.0f);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 8), 255);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 5.4), 0);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 5.0), 0);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 4.6), 0);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 4.5), 1);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 4.2), 1);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 3.9), 1);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 3.8), 1);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 3.5), 1);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 3.2), 1);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 3.1), 3);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 2.9), 3);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 2.7), 3);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 2.6), 2);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 2.4), 2);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 2.3), 2);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 2.2), 5);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 2.0), 5);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 1.9), 5);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 1.8), 3);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 1.7), 3);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 1.6), 3);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 1.5), 7);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 1.4), 7);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 1.3), 7);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 1.2), 255);

  dmn = dMinForScale(3 /* 5 */, 5.0f);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 20), 255);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 13.7), 255);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 13.6), 0);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 12.4), 1);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 11.4), 1);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 11.3), 1);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 10.4), 1);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 9.5), 1);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 9.4), 1);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 8.6), 2);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 7.9), 2);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 7.8), 3);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 7.2), 3);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 6.6), 3);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 6.5), 2);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 6.0), 3);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 5.5), 3);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 5.4), 5);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 5.0), 5);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 4.6), 5);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 4.5), 3);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 4.2), 3);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 3.9), 255);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 3.8), 7);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 3.5), 7);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 3.2), 7);
  EXPECT_FLOAT_EQ(scaleForDist(dmn, 3.1), 255);

  // dmn = dMinForScale(7, 5.0f);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 19.7), 255);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 19.6), 0);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 17.9), 0);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 16.4), 0);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 16.3), 1);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 14.9), 1);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 13.7), 1);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 13.6), 2);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 12.4), 2);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 11.4), 2);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 11.3), 3);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 10.4), 3);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 9.5), 3);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 9.4), 4);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 8.6), 4);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 7.9), 4);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 7.8), 5);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 7.2), 5);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 6.6), 5);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 6.5), 6);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 6.0), 6);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 5.5), 6);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 5.4), 7);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 5.0), 7);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 4.6), 7);
  // EXPECT_FLOAT_EQ(scaleForDist(dmn, 4.5), 255);
}

}  // namespace c8
