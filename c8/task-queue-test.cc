// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":task-queue",
    ":thread-pool",
    "//c8:vector",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x0874252b);

#include "c8/thread-pool.h"

#include <numeric>
#include <thread>

#include "c8/task-queue.h"
#include "c8/vector.h"

#include "gtest/gtest.h"

namespace c8 {

class TaskQueueTest : public ::testing::Test {};

TEST_F(TaskQueueTest, TestExecute) {
  TaskQueue queue;

  // Initialize a vector of 10000 zeros.
  Vector<int> vec(10000, 0);

  auto flipToOne = [&vec](int index) { vec[index] = 1; };

  // Add tasks to flip every even index to 1.
  for (int i = 0; i < vec.size(); i += 2) {
    queue.addTask(std::bind(flipToOne, i));
  }

  // Execute the tasks.
  queue.execute();

  EXPECT_EQ(1, vec[0]);
  EXPECT_EQ(0, vec[1]);
  EXPECT_EQ(1, vec[2]);
  EXPECT_EQ(0, vec[3]);
  EXPECT_EQ(1, vec[9998]);
  EXPECT_EQ(0, vec[9999]);

  EXPECT_EQ(5000, std::accumulate(vec.cbegin(), vec.cend(), 0));
}

TEST_F(TaskQueueTest, TestExecuteWithThreadPool) {
  int nthreads = std::min(1ul, std::thread::hardware_concurrency() - 1ul);
  ThreadPool pool(nthreads);
  TaskQueue queue;

  // Initialize a vector of 10000 zeros.
  Vector<int> vec(10000, 0);

  auto flipToOne = [&vec](int index) { vec[index] = 1; };

  // Add tasks to flip every even index to 1.
  for (int i = 0; i < vec.size(); i += 2) {
    queue.addTask(std::bind(flipToOne, i));
  }

  // Execute the tasks.
  queue.executeWithThreadPool(&pool);

  EXPECT_EQ(1, vec[0]);
  EXPECT_EQ(0, vec[1]);
  EXPECT_EQ(1, vec[2]);
  EXPECT_EQ(0, vec[3]);
  EXPECT_EQ(1, vec[9998]);
  EXPECT_EQ(0, vec[9999]);

  EXPECT_EQ(5000, std::accumulate(vec.cbegin(), vec.cend(), 0));
}

TEST_F(TaskQueueTest, RunItBackWithThreadPool) {
  int nthreads = std::min(1ul, std::thread::hardware_concurrency() - 1ul);
  ThreadPool pool(nthreads);
  TaskQueue queue;

  // Initialize a vector of 10000 zeros.
  Vector<int> vec(10000, 0);

  auto flipToOne = [&vec](int index) { vec[index] = 1; };

  // Add tasks to flip every even index to 1.
  for (int i = 0; i < vec.size(); i += 2) {
    queue.addTask(std::bind(flipToOne, i));
  }

  // Execute the tasks.
  queue.executeWithThreadPool(&pool);

  auto flipToTwo = [&vec](int index) { vec[index] = 2; };

  // Add tasks to flip every odd index to 2.
  for (int i = 1; i < vec.size(); i += 2) {
    queue.addTask(std::bind(flipToTwo, i));
  }

  // Execute the new set of tasks.
  queue.executeWithThreadPool(&pool);

  EXPECT_EQ(1, vec[0]);
  EXPECT_EQ(2, vec[1]);
  EXPECT_EQ(1, vec[2]);
  EXPECT_EQ(2, vec[3]);
  EXPECT_EQ(1, vec[9998]);
  EXPECT_EQ(2, vec[9999]);

  EXPECT_EQ(15000, std::accumulate(vec.cbegin(), vec.cend(), 0));
}

}  // namespace c8
