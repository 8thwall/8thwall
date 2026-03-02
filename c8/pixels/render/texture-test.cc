// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":texture",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x432c89f2);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/pixels/render/texture.h"

namespace c8 {

class TextureTest : public ::testing::Test {};

TEST_F(TextureTest, TestNativeId) {
  auto tex = TexGen::nativeId(42);
  // Check basic stats.
  EXPECT_EQ(42, tex->nativeId());
}

}  // namespace c8
