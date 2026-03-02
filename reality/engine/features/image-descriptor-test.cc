// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":image-descriptor",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x977ae4d0);

#include "reality/engine/features/image-descriptor.h"

#include "gtest/gtest.h"

namespace c8 {

class ImageDescriptorTestTest : public ::testing::Test {};

TEST_F(ImageDescriptorTestTest, TestDistance) {
  ImageDescriptor<8> a(std::array<uint8_t, 8>{{0b000, 0b011, 0b110, 0b011, 0b100, 0b111, 0b010, 0b111}});
  ImageDescriptor<8> b(std::array<uint8_t, 8>{{0b111, 0b110, 0b101, 0b100, 0b011, 0b010, 0b101, 0b000}});

  EXPECT_EQ(a.hammingDistance(b), 21);
  EXPECT_EQ(a.hammingDistance(a), 0);
}

TEST_F(ImageDescriptorTestTest, TestClone) {
  ImageDescriptor32 a = {};
  EXPECT_EQ(a.hammingDistance(a.clone()), 0);
}

}  // namespace c8
