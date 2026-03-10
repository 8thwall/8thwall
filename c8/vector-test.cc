// Copyright (c) 2016 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":vector",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xdc739ba0);

#include "c8/vector.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ContainerEq;

namespace c8 {

class VectorTest : public ::testing::Test {};

TEST_F(VectorTest, CreateAndDestroyVector) {
  Vector<int> vec{1, 2, 3, 4};
  EXPECT_THAT(vec, ContainerEq(Vector<int>{1, 2, 3, 4}));
}

}  // namespace c8
