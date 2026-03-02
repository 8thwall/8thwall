// Copyright (c) 2023 8th Wall, LLC.
// Original Author: Pawel Czarnecki (pawel@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":scope-exit",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x2a5a695b);

#include "c8/scope-exit.h"
#include "gmock/gmock.h"

namespace c8 {

class ScopeExitTest : public ::testing::Test {};

class MoveOnly {
public:
  explicit MoveOnly() = default;

  // disallow copy.
  MoveOnly(const MoveOnly &) = delete;
  MoveOnly &operator=(const MoveOnly &) = delete;

  // allow move.
  MoveOnly(MoveOnly &&) = default;
  MoveOnly &operator=(MoveOnly &&) = default;

  int num{0};
};

class CopyOnly {
public:
  explicit CopyOnly() = default;

  // allow copy.
  CopyOnly(const CopyOnly &) = default;
  CopyOnly &operator=(const CopyOnly &) = default;

  // disallow move.
  CopyOnly(CopyOnly &&) = delete;
  CopyOnly &operator=(CopyOnly &&) = delete;

  int num{0};
};

TEST_F(ScopeExitTest, function) {
  CopyOnly foo;  // MoveOnly is a compile error;
  bool called = false;
  {
    ScopeExit([foo, &called]() mutable {
      foo.num = 42;
      called = true;
    });
  }
  EXPECT_TRUE(called);
  EXPECT_EQ(foo.num, 0);
}

TEST_F(ScopeExitTest, scheduled_task) {
  MoveOnly foo;  // CopyOnly is a compile error.
  bool called = false;
  {
    ScopeExit(PackagedTask([f = std::move(foo), &called]() mutable {
      f.num = 42;
      called = true;
    }));
  }
  EXPECT_TRUE(called);
  EXPECT_EQ(foo.num, 0);
}

}  // namespace c8
