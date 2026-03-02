// Copyright (c) 2016 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":string",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xfd1425d3);

#include "c8/string.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::StrEq;

namespace c8 {

class StringTest : public ::testing::Test {};

TEST_F(StringTest, CreateAndDestroyString) {
  String str = "awesome";
  EXPECT_THAT(str, StrEq("awesome"));
}

}  // namespace c8
