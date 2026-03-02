// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":split",
    "//c8:string",
    "//c8:string-view",
    "//c8:vector",
    "//c8/string:format",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xe96c3f27);

#include <string>

#include "c8/string-view.h"
#include "c8/string.h"
#include "c8/string/format.h"
#include "c8/string/split.h"
#include "c8/vector.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Eq;
using testing::Pointwise;
using ::testing::StrEq;

namespace c8 {

class SplitTest : public ::testing::Test {};

MATCHER(AreEqual, "") { return testing::get<0>(arg) == testing::get<1>(arg); }
decltype(auto) equalsVector(const Vector<String> &v) { return Pointwise(AreEqual(), v); }

TEST_F(SplitTest, Main) {
  // Can split correctly.
  EXPECT_THAT(split("a,a,a,a", ","), equalsVector({"a", "a", "a", "a"}));
  EXPECT_THAT(split("a,a a,a", " "), equalsVector({"a,a", "a,a"}));

  // Don't split incorrectly.
  EXPECT_THAT(split("", ","), equalsVector({""}));
  EXPECT_THAT(split("a,a,a,a", "."), equalsVector({"a,a,a,a"}));
  EXPECT_THAT(split("a,a,a,a", "."), equalsVector({"a,a,a,a"}));
  EXPECT_THAT(split("a,a,a,a", ""), equalsVector({"a,a,a,a"}));

  // Handle trailing delimiter.
  EXPECT_THAT(split("a a ", " "), equalsVector({"a", "a", ""}));
  EXPECT_THAT(split("ab ab ", " "), equalsVector({"ab", "ab", ""}));

  // Split on string itself.
  EXPECT_THAT(split("a", "a"), equalsVector({"", ""}));
  EXPECT_THAT(split("ab", "ab"), equalsVector({"", ""}));
  EXPECT_THAT(split("abc", "abc"), equalsVector({"", ""}));
}

}  // namespace c8
