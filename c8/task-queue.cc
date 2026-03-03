// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "task-queue.h",
  };
  visibility = {
    "//visibility:public",
  };
  deps = {
    ":thread-pool",
    "//c8:vector",
  };
}
cc_end(0xf5856d55);

#include "c8/task-queue.h"

#include "c8/thread-pool.h"

#include <atomic>

namespace c8 {

void TaskQueue::execute() {
  for (auto &task : tasks_) {
    task();
  }
  tasks_.clear();
}

void TaskQueue::executeWithThreadPool(ThreadPool *pool) {
  std::atomic<int> index(0);

  auto work = [&, this]() {
    int localIndex = index.fetch_add(1, std::memory_order_relaxed);
    if (localIndex >= tasks_.size()) {
      return false;
    } else {
      auto &task = tasks_.at(localIndex);
      task();
      return true;
    }
  };

  auto poolBarrier = pool->execute(work);

  while (work()) {
    // The calling thread also participates in the work.
  }

  // Wait for the thread pool work to complete.
  poolBarrier.get();

  tasks_.clear();
}

}  // namespace c8
