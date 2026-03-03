// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":strcat",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xece27c6b);

#include "c8/string/strcat.h"

#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::StrEq;

namespace c8 {

class StrcatTest : public ::testing::Test {};

TEST_F(StrcatTest, strCat) {
  EXPECT_THAT(strCat("1", "2", "3"), StrEq("123"));
  EXPECT_THAT(strCat(std::string("1"), "2", String("3")), StrEq("123"));
  EXPECT_THAT(strCat("5", "+", 3, "=", 8), StrEq("5+3=8"));
  EXPECT_THAT(strCat("1.0/3.0 = ", 1.0/3.0), StrEq("1.0/3.0 = 0.333333"));
}

}  // namespace c8
