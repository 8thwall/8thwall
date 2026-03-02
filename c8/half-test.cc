// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)
//
// Unit tests for half precision (16 bit) floating point numbers.

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":half", 
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x80a7dda4);

#include <cmath>
#include <cmath>
#include <limits>

#include "c8/half.h"

#include "gtest/gtest.h"

namespace c8 {

class HalfTest : public ::testing::Test {};

TEST_F(HalfTest, HalfToFloat) {
  // Test cases from:
  // https://en.wikipedia.org/wiki/Half-precision_floating-point_format#Half_precision_examples

  // Zero
  EXPECT_EQ(halfToFloat(0x0000), 0.0f);
  EXPECT_EQ(halfToFloat(0x8000), -0.0f);

  // Subnormal numbers
  EXPECT_EQ(halfToFloat(0x0001), std::pow(2.0f, -14.0f) * (0.0f + 1.0f / 1024.0f));
  EXPECT_EQ(halfToFloat(0x03FF), std::pow(2.0f, -14.0f) * (0.0f + 1023.0f / 1024.0f));
  EXPECT_EQ(halfToFloat(0x8001), -std::pow(2.0f, -14.0f) * (0.0f + 1.0f / 1024.0f));
  EXPECT_EQ(halfToFloat(0x83FF), -std::pow(2.0f, -14.0f) * (0.0f + 1023.0f / 1024.0f));

  // Smallest normal number:
  EXPECT_EQ(halfToFloat(0x0400), std::pow(2.0f, -14.0f) * (1.0f + 0.0f / 1024.0f));
  EXPECT_EQ(halfToFloat(0x8400), -std::pow(2.0f, -14.0f) * (1.0f + 0.0f / 1024.0f));

  // Closest number to 1/3:
  EXPECT_EQ(halfToFloat(0x3555), std::pow(2.0f, -2.0f) * (1.0f + 341.0f / 1024.0f));
  EXPECT_EQ(halfToFloat(0xB555), -std::pow(2.0f, -2.0f) * (1.0f + 341.0f / 1024.0f));

  // Largest number less than 1:
  EXPECT_EQ(halfToFloat(0x3BFF), std::pow(2.0f, -1.0f) * (1.0f + 1023.0f / 1024.0f));
  EXPECT_EQ(halfToFloat(0xBBFF), -std::pow(2.0f, -1.0f) * (1.0f + 1023.0f / 1024.0f));

  // 1:
  EXPECT_EQ(halfToFloat(0x3C00), std::pow(2.0f, 0.0f) * (1.0f + 0.0f / 1024.0f));
  EXPECT_EQ(halfToFloat(0xBC00), -std::pow(2.0f, 0.0f) * (1.0f + 0.0f / 1024.0f));

  // Smallest number greater than 1:
  EXPECT_EQ(halfToFloat(0x3C01), std::pow(2.0f, 0.0f) * (1.0f + 1.0f / 1024.0f));
  EXPECT_EQ(halfToFloat(0xBC01), -std::pow(2.0f, 0.0f) * (1.0f + 1.0f / 1024.0f));

  // 2:
  EXPECT_EQ(halfToFloat(0x4000), std::pow(2.0f, 1.0f) * (1.0f + 0.0f / 1024.0f));
  EXPECT_EQ(halfToFloat(0xC000), -std::pow(2.0f, 1.0f) * (1.0f + 0.0f / 1024.0f));

  // Largest normal number:
  EXPECT_EQ(halfToFloat(0x7BFF), std::pow(2.0f, 15.0f) * (1.0f + 1023.0f / 1024.0f));
  EXPECT_EQ(halfToFloat(0xFBFF), -std::pow(2.0f, 15.0f) * (1.0f + 1023.0f / 1024.0f));

  // Infinity
  EXPECT_EQ(halfToFloat(0x7C00), std::numeric_limits<float>::infinity());
  EXPECT_EQ(halfToFloat(0xFC00), -std::numeric_limits<float>::infinity());

  // NaN
  EXPECT_TRUE(std::isnan(halfToFloat(0x7C01)));
  EXPECT_TRUE(std::isnan(halfToFloat(0x7FFF)));
  EXPECT_TRUE(std::isnan(halfToFloat(0xFFFF)));
}

TEST_F(HalfTest, FloatToHalf) {
  // Test cases from:
  // https://en.wikipedia.org/wiki/Half-precision_floating-point_format#Half_precision_examples

  // Zero
  EXPECT_EQ(floatToHalf(0.0f), 0x0000);
  EXPECT_EQ(floatToHalf(-0.0f), 0x8000);

  // Subnormal numbers
  EXPECT_EQ(floatToHalf(std::pow(2.0f, -14.0f) * (0.0f + 1.0f / 1024.0f)), 0x0001);
  EXPECT_EQ(floatToHalf(std::pow(2.0f, -14.0f) * (0.0f + 1023.0f / 1024.0f)), 0x03FF);
  EXPECT_EQ(floatToHalf(-std::pow(2.0f, -14.0f) * (0.0f + 1.0f / 1024.0f)), 0x8001);
  EXPECT_EQ(floatToHalf(-std::pow(2.0f, -14.0f) * (0.0f + 1023.0f / 1024.0f)), 0x83FF);

  // Smallest normal number:
  EXPECT_EQ(floatToHalf(std::pow(2.0f, -14.0f) * (1.0f + 0.0f / 1024.0f)), 0x0400);
  EXPECT_EQ(floatToHalf(-std::pow(2.0f, -14.0f) * (1.0f + 0.0f / 1024.0f)), 0x8400);

  // Closest number to 1/3:
  EXPECT_EQ(floatToHalf(std::pow(2.0f, -2.0f) * (1.0f + 341.0f / 1024.0f)), 0x3555);
  EXPECT_EQ(floatToHalf(-std::pow(2.0f, -2.0f) * (1.0f + 341.0f / 1024.0f)), 0xB555);

  // Largest number less than 1:
  EXPECT_EQ(floatToHalf(std::pow(2.0f, -1.0f) * (1.0f + 1023.0f / 1024.0f)), 0x3BFF);
  EXPECT_EQ(floatToHalf(-std::pow(2.0f, -1.0f) * (1.0f + 1023.0f / 1024.0f)), 0xBBFF);

  // 1:
  EXPECT_EQ(floatToHalf(std::pow(2.0f, 0.0f) * (1.0f + 0.0f / 1024.0f)), 0x3C00);
  EXPECT_EQ(floatToHalf(-std::pow(2.0f, 0.0f) * (1.0f + 0.0f / 1024.0f)), 0xBC00);

  // Smallest number greater than 1:
  EXPECT_EQ(floatToHalf(std::pow(2.0f, 0.0f) * (1.0f + 1.0f / 1024.0f)), 0x3C01);
  EXPECT_EQ(floatToHalf(-std::pow(2.0f, 0.0f) * (1.0f + 1.0f / 1024.0f)), 0xBC01);

  // 2:
  EXPECT_EQ(floatToHalf(std::pow(2.0f, 1.0f) * (1.0f + 0.0f / 1024.0f)), 0x4000);
  EXPECT_EQ(floatToHalf(-std::pow(2.0f, 1.0f) * (1.0f + 0.0f / 1024.0f)), 0xC000);

  // Largest normal number:
  EXPECT_EQ(floatToHalf(std::pow(2.0f, 15.0f) * (1.0f + 1023.0f / 1024.0f)), 0x7BFF);
  EXPECT_EQ(floatToHalf(-std::pow(2.0f, 15.0f) * (1.0f + 1023.0f / 1024.0f)), 0xFBFF);
  
  // Infinity
  EXPECT_EQ(floatToHalf(std::numeric_limits<float>::infinity()), 0x7C00);
  EXPECT_EQ(floatToHalf(-std::numeric_limits<float>::infinity()), 0xFC00);

  // Nan
  EXPECT_EQ(floatToHalf(0.0f / 0.0f), 0x7C01);

  // Below range equals zero:
  EXPECT_EQ(floatToHalf(std::pow(2.0f, -14.0f) * (0.0f + 1.0f / 2048.0f)), 0x0000);
  EXPECT_EQ(floatToHalf(-std::pow(2.0f, -14.0f) * (0.0f + 1.0f / 2048.0f)), 0x8000);

  // Above range equals inf:
  EXPECT_EQ(floatToHalf(std::pow(2.0f, 16.0f) * (1.0f + 0.0f / 1024.0f)), 0x7C00);
  EXPECT_EQ(floatToHalf(-std::pow(2.0f, 16.0f) * (1.0f + 0.0f / 1024.0f)), 0xFC00);
}

TEST_F(HalfTest, Pack2) {
  auto packed = packHalf2x16({2.0f, 3.0f});
  auto [a, b] = unpackHalf2x16(packed);
  EXPECT_EQ(a, 2.0f);
  EXPECT_EQ(b, 3.0f);
}

}  // namespace c8
