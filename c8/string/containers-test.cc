// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Nicholas Butko (nb)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":containers",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x3f99868c);

#include "c8/string/containers.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

class ContainersTest : public ::testing::Test {};

// Test vector transform.
TEST_F(ContainersTest, Transform) {
  Vector<int> v = {0, 1, 2};
  auto tv = map<int, int>(v, [](const auto &i) { return i + 1;});
  EXPECT_EQ(1, tv[0]);
  EXPECT_EQ(2, tv[1]);
  EXPECT_EQ(3, tv[2]);
}

}  // namespace c8
