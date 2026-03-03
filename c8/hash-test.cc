// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":hash",
    ":string",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xd523faa5);

#include <cmath>

#include "c8/hash.h"
#include "c8/string.h"
#include "gtest/gtest.h"
namespace c8 {

class HashTest : public ::testing::Test {};

TEST_F(HashTest, NotEqual) {
  uint32_t s1 = 0;
  murmurHash32Combine<float>(1.f, &s1);
  murmurHash32Combine<float>(2.f, &s1);

  uint32_t s2 = 0;
  murmurHash32Combine<float>(3.f, &s2);

  EXPECT_NE(s1, s2);
}

TEST_F(HashTest, Equal) {
  uint32_t s1 = 0;
  murmurHash32Combine<float>(1.f, &s1);
  murmurHash32Combine<float>(2.f, &s1);

  uint32_t s2 = 0;
  murmurHash32Combine<float>(1.f, &s2);
  murmurHash32Combine<float>(2.f, &s2);

  EXPECT_EQ(s1, s2);
}

TEST_F(HashTest, OrderMatters) {
  uint32_t s1 = 0;
  murmurHash32Combine<float>(1.f, &s1);
  murmurHash32Combine<float>(2.f, &s1);

  uint32_t s2 = 0;
  murmurHash32Combine<float>(2.f, &s2);
  murmurHash32Combine<float>(1.f, &s2);

  EXPECT_NE(s1, s2);
}

TEST_F(HashTest, Primitives) {
  uint32_t s1 = 0;
  murmurHash32Combine<float>(1.f, &s1);
  uint32_t s1b = 0;
  murmurHash32Combine<float>(1.f, &s1b);
  EXPECT_EQ(s1, s1b);

  uint32_t s2 = 0;
  murmurHash32Combine<int>(1, &s2);
  uint32_t s2b = 0;
  murmurHash32Combine<int>(1, &s2b);
  EXPECT_EQ(s2, s2b);

  uint32_t s3 = 0;
  murmurHash32Combine<char>('a', &s3);
  uint32_t s3b = 0;
  murmurHash32Combine<char>('a', &s3b);
  EXPECT_EQ(s3, s3b);

  uint32_t s4 = 0;
  murmurHash32Combine<bool>(true, &s4);
  uint32_t s4b = 0;
  murmurHash32Combine<bool>(true, &s4b);
  EXPECT_EQ(s4, s4b);

  EXPECT_NE(s1, s2);
  EXPECT_NE(s1, s3);
  EXPECT_NE(s1, s4);
  EXPECT_NE(s2, s3);
  EXPECT_NE(s2, s4);  // An int of `1` and a bool of `true` are equal.
  EXPECT_NE(s3, s4);
}

struct S {
  int a;
  bool b;
  float c;
};

struct S2 {
  int x;
  bool y;
  float z;
};

TEST_F(HashTest, Structs) {
  S obj = {0, true, 2.f};
  uint32_t s1 = 0;
  murmurHash32Combine<S>(obj, &s1);

  // Another struct has same hash.
  uint32_t s2 = 0;
  murmurHash32Combine<S>(obj, &s2);
  EXPECT_EQ(s1, s2);

  // Different hash if you modify a value.
  obj.a = 1;
  s2 = 0;
  murmurHash32Combine<S>(obj, &s2);
  EXPECT_NE(s1, s2);

  // Same hash if you go back.
  obj.a = 0;
  s2 = 0;
  murmurHash32Combine<S>(obj, &s2);
  EXPECT_EQ(s1, s2);

  // Different object with the same values has the same hash.
  S obj2 = {0, true, 2.f};
  s2 = 0;
  murmurHash32Combine<S>(obj2, &s2);
  EXPECT_EQ(s1, s2);
}

TEST_F(HashTest, CombinedStructs) {
  S obj = {0, true, 2.f};
  S2 obj2 = {0, true, 2.f};

  uint32_t s1 = 0;
  murmurHash32Combine<S>(obj, &s1);
  murmurHash32Combine<S2>(obj2, &s1);

  uint32_t s2 = 0;
  murmurHash32Combine<S>(obj, &s2);
  murmurHash32Combine<S2>(obj2, &s2);

  EXPECT_EQ(s1, s2);
}

TEST_F(HashTest, HashString) {
  String str = "hello";
  auto s1 = murmurHash32(str.data(), str.size());
  auto s2 = murmurHash32(str.data(), str.size());
  EXPECT_EQ(s1, s2);

  String str1 = "hello1";
  auto s3 = murmurHash32(str.c_str(), str1.size());
  EXPECT_NE(s2, s3);
}

}  // namespace c8
