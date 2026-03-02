// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":float-vector",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x00bde77b);

#include "c8/float-vector.h"

#include <algorithm>
#include <cmath>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAre;

namespace c8 {

class FloatVectorTest : public ::testing::Test {};

TEST_F(FloatVectorTest, Addition) {
  FloatVector a = {1, 2, 3, 4};
  FloatVector b = {5, 6, 7, 8};

  FloatVector c = a + b;

  EXPECT_THAT(c, ElementsAre(6, 8, 10, 12));

  c += a;
  EXPECT_THAT(c, ElementsAre(7, 10, 13, 16));

  c += 2;
  EXPECT_THAT(c, ElementsAre(9, 12, 15, 18));

  auto d = 4.0f + c;
  EXPECT_THAT(d, ElementsAre(13, 16, 19, 22));

  auto e = c + 4.0f;
  EXPECT_THAT(e, ElementsAre(13, 16, 19, 22));
}

TEST_F(FloatVectorTest, Subtraction) {
  FloatVector a = {1, 2, 3, 4};
  FloatVector b = {5, 6, 7, 8};

  FloatVector c = a - b;

  EXPECT_THAT(c, ElementsAre(-4, -4, -4, -4));

  c -= a;
  EXPECT_THAT(c, ElementsAre(-5, -6, -7, -8));

  c -= 2;
  EXPECT_THAT(c, ElementsAre(-7, -8, -9, -10));

  FloatVector e = c - 4.0f;
  EXPECT_THAT(e, ElementsAre(-11, -12, -13, -14));
}

TEST_F(FloatVectorTest, Multiplication) {
  FloatVector a = {1, 2, 3, 4};
  FloatVector b = {5, 6, 7, 8};

  FloatVector c = a * b;

  EXPECT_THAT(c, ElementsAre(5, 12, 21, 32));

  c *= a;
  EXPECT_THAT(c, ElementsAre(5, 24, 63, 128));

  c *= 2;
  EXPECT_THAT(c, ElementsAre(10, 48, 126, 256));

  auto d = 4.0f * c;
  EXPECT_THAT(d, ElementsAre(40, 192, 504, 1024));

  auto e = c * 4.0f;
  EXPECT_THAT(d, ElementsAre(40, 192, 504, 1024));
}

TEST_F(FloatVectorTest, CloneCopyMove) {
  FloatVector a = {1, 2, 3, 4};

  FloatVector b = a.clone();

  FloatVector c;
  c.copyFrom(a);

  EXPECT_THAT(b, ElementsAre(1, 2, 3, 4));
  EXPECT_THAT(c, ElementsAre(1, 2, 3, 4));

  FloatVector d = std::move(a);
  EXPECT_THAT(d, ElementsAre(1, 2, 3, 4));
}

TEST_F(FloatVectorTest, Distances) {
  FloatVector a = {1, 2, 3, 4};
  FloatVector b = {5, 6, 7, 8};

  EXPECT_FLOAT_EQ(16.0f, l1Distance(a, b));
  EXPECT_FLOAT_EQ(16.0f, l1Distance(b, a));
  EXPECT_FLOAT_EQ(8.0f, l2Distance(a, b));
  EXPECT_FLOAT_EQ(8.0f, l2Distance(b, a));
}

TEST_F(FloatVectorTest, Norms) {
  FloatVector a = {1, -2, 3, -4};

  EXPECT_FLOAT_EQ(10.0f, l1Norm(a));
  EXPECT_FLOAT_EQ(30.0f, l2SquaredNorm(a));
  EXPECT_FLOAT_EQ(std::sqrt(30.0f), l2Norm(a));
}

TEST_F(FloatVectorTest, Normalize) {
  FloatVector a = {1, -2, 3, -4};
  EXPECT_THAT(a.l1Normalize(), ElementsAre(.1, -.2, .3, -.4));

  FloatVector b = {3, -4};
  EXPECT_THAT(b.l2Normalize(), ElementsAre(0.6f, -0.8f));
}

TEST_F(FloatVectorTest, Sqrt) {
  FloatVector a = {1, 2, 3, 4};
  EXPECT_THAT(a.sqrt(), ElementsAre(1, std::sqrt(2), std::sqrt(3), 2));

  FloatVector b = {1, -2, 3, -4};
  EXPECT_THAT(b.signSqrt(), ElementsAre(1, -std::sqrt(2), std::sqrt(3), -2));
}

TEST_F(FloatVectorTest, InnerProduct) {
  FloatVector a = {1, -2, 3, -4};
  FloatVector b = {5, 6, 7, 8};

  EXPECT_FLOAT_EQ(-18.0f, innerProduct(a, b));
  EXPECT_FLOAT_EQ(-18.0f, innerProduct(b, a));
}

TEST_F(FloatVectorTest, Invert) {
  FloatVector a = {4, -1, -2, 10};
  EXPECT_THAT(a.invert(), ElementsAre(0.25, -1.0, -.5, 0.1));
}

TEST_F(FloatVectorTest, Mean) {
  FloatVector a = {4, -1, -2, 10};
  EXPECT_FLOAT_EQ(mean(a), 2.75);
}

}  // namespace c8
