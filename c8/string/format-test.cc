// Copyright (c) 2022 Niantic Inc.
// Original Author: Paris Morgan (parismorgan@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//c8:string",
    ":format",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xe47ac229);

#include "c8/string.h"
#include "c8/string/format.h"
#include "gtest/gtest.h"

namespace c8 {

namespace {

struct Foo {
  String name = "foo";
  friend std::ostream &operator<<(std::ostream &stream, const Foo &foo) {
    return stream << foo.name;
  }
};

struct Bar {
  String toString() const { return "bar@toString"; }
};

struct FooBar {
  String name = "fooBar";
  friend std::ostream &operator<<(std::ostream &stream, const FooBar &fooBar) {
    return stream << fooBar.name;
  }
  String toString() const { return "fooBar@toString"; }
};

}  // namespace

class FormatTest : public ::testing::Test {};

TEST_F(FormatTest, ToUpperCase) {
  String s = "Hello there";
  EXPECT_STREQ("HELLO THERE", toUpperCase(s).c_str());

  s = "HELLO THERE";
  EXPECT_STREQ("HELLO THERE", toUpperCase(s).c_str());

  s = "123!";
  EXPECT_STREQ("123!", toUpperCase(s).c_str());
}

TEST_F(FormatTest, BoolToString) {
  EXPECT_STREQ("true", boolToString(true).c_str());
  EXPECT_STREQ("false", boolToString(false).c_str());
}

TEST_F(FormatTest, ToString) {
  Foo foo;
  EXPECT_STREQ("foo", toString(foo).c_str());
  Vector<Foo> foos{{}, {}};
  EXPECT_STREQ("[foo, foo]", toString<Foo>(foos).c_str());

  // If both operator<<() and toString() are defined, prefer toString().
  Vector<FooBar> fooBars{{}, {}};
  EXPECT_STREQ("[fooBar@toString, fooBar@toString]", toString<FooBar>(fooBars).c_str());

  Bar bar;
  Vector<Bar> bars{{}, {}};
  EXPECT_STREQ("[bar@toString, bar@toString]", toString<Bar>(bars).c_str());
}

}  // namespace c8
