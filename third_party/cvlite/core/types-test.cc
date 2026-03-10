// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":core",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x90cf7509);

#include "third_party/cvlite/core/types.hpp"

#include "gtest/gtest.h"

namespace c8 {

class TypesTest : public ::testing::Test {};

TEST_F(TypesTest, BasicTypesAreTriviallyCopyable) {
  EXPECT_TRUE(std::is_trivially_copyable_v<c8cv::Size>);
  EXPECT_TRUE(std::is_trivially_copyable_v<c8cv::Point>);
}

}  // namespace c8
