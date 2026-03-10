// Copyright (c) 2021 Niantic Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//c8:string",
    ":contains",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xb5a8bad1);

#include "c8/string.h"
#include "c8/string/contains.h"
#include "gtest/gtest.h"

namespace c8 {

class ContainsTest : public ::testing::Test {};

TEST_F(ContainsTest, StartsWith) {
  String foo = "Hello there";
  EXPECT_TRUE(startsWith(foo, ""));
  EXPECT_TRUE(startsWith(foo, "Hello"));
  EXPECT_TRUE(startsWith(foo, foo));
  EXPECT_FALSE(startsWith(foo, "hello")) << "Case matters";
}

// Test vector transform.
TEST_F(ContainsTest, EndsWith) {
  String foo = "Hello there";
  EXPECT_TRUE(endsWith(foo, ""));
  EXPECT_TRUE(endsWith(foo, "there"));
  EXPECT_TRUE(endsWith(foo, foo));
  EXPECT_FALSE(endsWith(foo, "There")) << "Case matters";
}

}  // namespace c8
