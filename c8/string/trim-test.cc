// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":trim",
    "//c8:string",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xd4c6c5ee);

#include "c8/string.h"
#include "c8/string/trim.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::StrEq;

namespace c8 {

class StringTest : public ::testing::Test {};

// Test trim removes leading and trailing newlines.
TEST_F(StringTest, StrTrimNewLine) {
  String str = "\n\n\n\nHello\nWorld!\n\n\n\n";
  EXPECT_THAT(strTrim(str), StrEq("Hello\nWorld!"));
}

// Test trim removes leading and trailing spaces.
TEST_F(StringTest, StrTrimSpace) {
  String str = "      Hello World!     ";
  EXPECT_THAT(strTrim(str), StrEq("Hello World!"));
}

// Test trim removes leading and trailing tab.
TEST_F(StringTest, StrTrimTab) {
  String str = "\t\t\tHello\tWorld!\t\t\t";
  EXPECT_THAT(strTrim(str), StrEq("Hello\tWorld!"));
}

// Test trim removes leading and trailing newlines, spaces, and tabs.
TEST_F(StringTest, StrTrimCombo) {
  String str = "\n \tHello World! \t\n";
  EXPECT_THAT(strTrim(str), StrEq("Hello World!"));
}

// Test trim returns empty string for strings with only trim characters.
TEST_F(StringTest, StrTrimEmptyResult) {
  String str = "\n\t   \t \n \t\t\t  ";
  EXPECT_THAT(strTrim(str), StrEq(""));
}

// Test trim returns empty string for empty input.
TEST_F(StringTest, StrEmptyTrimEmptyResult) {
  String str = "";
  EXPECT_THAT(strTrim(str), StrEq(""));
}
}  // namespace c8
