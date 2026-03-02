// Copyright (c) 2016 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":join",
    "//c8:string",
    "//c8:string-view",
    "//c8:vector",
    "//c8/string:format",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x8ce0d6ec);

#include "c8/string/join.h"

#include <string>

#include "c8/string-view.h"
#include "c8/string.h"
#include "c8/string/format.h"
#include "c8/vector.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::StrEq;

namespace c8 {

class StringTest : public ::testing::Test {};

// Test strJoin on variations of a vector of strings.
TEST_F(StringTest, StrJoinVectorString) {
  Vector<String> strArray = {"Hello", "World!\n"};
  const Vector<String> constStrArray = {"foo", "bar", "baz"};

  Vector<const String> strConstArray = {"1", "2", "3", "4"};
  const Vector<const String> constStrConstArray = {"a", "b", "c", "d"};

  EXPECT_THAT(strJoin(strArray, " "), StrEq("Hello World!\n"));
  EXPECT_THAT(strJoin(strArray.cbegin(), strArray.cend(), " "), StrEq("Hello World!\n"));
  EXPECT_THAT(strJoin(strArray.begin(), strArray.end(), " "), StrEq("Hello World!\n"));

  EXPECT_THAT(strJoin(constStrArray, "--"), StrEq("foo--bar--baz"));
  EXPECT_THAT(strJoin(constStrArray.cbegin(), constStrArray.cend(), "::"), StrEq("foo::bar::baz"));
  EXPECT_THAT(strJoin(constStrArray.begin(), constStrArray.end(), "<>"), StrEq("foo<>bar<>baz"));

  EXPECT_THAT(strJoin(strConstArray, " "), StrEq("1 2 3 4"));
  EXPECT_THAT(strJoin(strConstArray.cbegin(), strConstArray.cend(), ","), StrEq("1,2,3,4"));
  EXPECT_THAT(strJoin(strConstArray.begin(), strConstArray.end(), ", "), StrEq("1, 2, 3, 4"));

  EXPECT_THAT(strJoin(constStrConstArray, "*"), StrEq("a*b*c*d"));
  EXPECT_THAT(
    strJoin(constStrConstArray.cbegin(), constStrConstArray.cend(), "_"), StrEq("a_b_c_d"));
  EXPECT_THAT(strJoin(constStrConstArray.begin(), constStrConstArray.end(), ">"), StrEq("a>b>c>d"));
}

// Test strJoin with char arrays.
TEST_F(StringTest, StrJoinCharArray) {
  constexpr const char *items[] = {"one", "two", "three"};
  constexpr int numbers[] = {3, 2, 1};
  EXPECT_THAT(strJoin(items, std::end(items), ", "), StrEq("one, two, three"));
  EXPECT_THAT(
    strJoin(numbers, std::end(numbers), " > ", [](int x) { return std::to_string(x); }),
    StrEq("3 > 2 > 1"));
}

// Test strJoin with arithmetic types.
TEST_F(StringTest, StrJoinVectorNumber) {
  EXPECT_THAT(strJoin({1, 2, 3}, " < "), StrEq("1 < 2 < 3"));
  EXPECT_THAT(strJoin({1.4, 1.6, 2.2}, " + "), StrEq("1.4 + 1.6 + 2.2"));
}

// Test strJoin with formatting lambdas.
TEST_F(StringTest, StrJoinVectorNumberLambda) {
  EXPECT_THAT(
    strJoin({1, 2, 3}, " < ", [](int x) { return std::to_string(x); }), StrEq("1 < 2 < 3"));
  EXPECT_THAT(
    strJoin({1.4, 1.6, 2.2}, " + ", [](double x) { return format("%0.1f", x); }),
    StrEq("1.4 + 1.6 + 2.2"));
}

// Test strJoin on an initializer list.
TEST_F(StringTest, StrJoinInitializerList) {
  EXPECT_THAT(strJoin({"1", "2", "3"}, " < "), StrEq("1 < 2 < 3"));
}

TEST_F(StringTest, StrJoinRawArrayFloats) {
  float items[5] = {.1, .2, .3, .4, .5};
  EXPECT_THAT(
    strJoin(items, items + 3, " < ", [](float x) { return std::to_string(x); }),
    StrEq("0.100000 < 0.200000 < 0.300000"));
}

}  // namespace c8
