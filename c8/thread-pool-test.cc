// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":thread-pool",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x19bf7ffc);

#include "c8/thread-pool.h"

#include <thread>

#include "gtest/gtest.h"

namespace c8 {

class ThreadPoolTest : public ::testing::Test {};

TEST_F(ThreadPoolTest, TestCountDown) {
  std::atomic<int> countDown(10000);
  std::atomic<int> countUp(0);

  ThreadPool pool(std::thread::hardware_concurrency());

  auto done = pool.execute([&countDown, &countUp] {
    if (--countDown < 0) {
      return false;
    } else {
      ++countUp;
      return true;
    }
  });

  // Wait for the pool to complete.
  done.get();

  EXPECT_EQ(countUp, 10000);
}

// Test reusing a ThreadPool.
TEST_F(ThreadPoolTest, RunItBack) {
  std::atomic<int> countDown(10000);
  std::atomic<int> countUp(0);

  ThreadPool pool(std::thread::hardware_concurrency());

  auto task = [&countDown, &countUp] {
    if (--countDown < 0) {
      return false;
    } else {
      ++countUp;
      return true;
    }
  };

  auto done = pool.execute(task);

  // Wait for the pool to complete.
  done.get();

  // Run it back.
  countDown = 10000;

  done = pool.execute(task);

  // Wait for the pool to complete the second quest.
  done.get();

  EXPECT_EQ(countUp, 20000);
}

}  // namespace c8
