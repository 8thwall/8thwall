// Copyright (c) 2022 Niantic, Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":packed",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x07482977)

#include "c8/pixels/packed.h"
#include "gtest/gtest.h"

namespace c8 {

class PackedTest : public ::testing::Test {};

TEST_F(PackedTest, EncodeDecodeFloatUint8t) {
  uint8_t x,y,z,w;
  float theFloat = 0.42889250633567028616881555337687f;
  encodeFloat01(theFloat, &x, &y, &z, &w);
  EXPECT_NEAR(theFloat, decodeFloat(x,y,z,w), 1e-8);
}

TEST_F(PackedTest, EncodeDecodeAcrossRange) {
  uint8_t x,y,z,w;
  for (int i = 1; i <= 10; i++) {
    float theFloat = 0.091237132f * i;
    encodeFloat01(theFloat, &x, &y, &z, &w);
    EXPECT_NEAR(theFloat, decodeFloat(x,y,z,w), 1e-8);
  }
}

}  // namespace c8
