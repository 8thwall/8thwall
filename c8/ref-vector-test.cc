// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":ref-vector",
    ":vector",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x021d12dc);

#include "c8/ref-vector.h"
#include "c8/vector.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

using testing::ElementsAre;
using std::ref;

class RefVectorTest : public ::testing::Test {};

TEST_F(RefVectorTest, TestRefVectorConstruction) {
  Vector<int> foo = {0, 1, 2, 3, 4, 5, 6, 7};
  RefVector<int> bar = {foo[1], foo[3], foo[4]};
  EXPECT_EQ(bar[0], 1);
  EXPECT_EQ(bar[1], 3);
  EXPECT_EQ(bar[2], 4);

  bar.push_back(foo[6]);
  EXPECT_EQ(bar[3], 6);

  // Ensure bar is made of references.
  foo[3] = 5;
  EXPECT_EQ(bar[1], 5);
  bar[2] = 10;
  EXPECT_EQ(foo[4], 10);
  bar[3] = 99;
  EXPECT_EQ(foo[6], 99);
}

TEST_F(RefVectorTest, TestCRefVectorConstruction) {
  Vector<int> foo = {0, 1, 2, 3, 4, 5, 6, 7};
  CRefVector<int> bar = {foo[1], foo[3], foo[4]};
  EXPECT_EQ(bar[0], 1);
  EXPECT_EQ(bar[1], 3);
  EXPECT_EQ(bar[2], 4);

  bar.push_back(foo[6]);
  EXPECT_EQ(bar[3], 6);

  // Ensure bar is made of references.
  foo[3] = 5;
  EXPECT_EQ(bar[1], 5);
}

}  // namespace c8
