// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "thread-pool.h",
  };
  visibility = {
    "//visibility:public",
  };
  deps = {
    "//c8:exceptions",
    "//c8:vector",
  };
}
cc_end(0x62294c77);

#include <numeric>

#include "c8/exceptions.h"
#include "c8/thread-pool.h"

namespace c8 {

ThreadPool::ThreadPool(int nthreads) : shutdown_(false), active_(std::max(nthreads, 1), 0) {
#ifdef JAVASCRIPT
  auto threadCount = 0;
#else
  auto threadCount = active_.size();
#endif
  threads_.reserve(threadCount);
  if (threadCount > 1) {
    for (int i = 0; i < threadCount; ++i) {
      threads_.emplace_back(
        [&](int threadNum) {
          while (1) {
            {
              // Acquire the mutex and check for shutdown or work.
              std::unique_lock<std::mutex> lock(mutex_);
              cv_.wait(lock, [&] { return shutdown_ || active_[threadNum]; });

              if (shutdown_) {
                return;
              }
            }

            while (taskFunc_()) {
              // Continue to execute until out of work.
            }

            {
              // Mark that work is complete.
              std::lock_guard<std::mutex> lock(mutex_);
              active_[threadNum] = 0;
              if (std::accumulate(active_.begin(), active_.end(), 0) == 0) {
                workDone_.set_value();
              }
            }
          }
        },
        i);
    }
  }
}

ThreadPool::~ThreadPool() {
  if (threads_.size() > 1) {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      shutdown_ = true;
    }
    cv_.notify_all();

    // Wait for all of the threads to shut down.
    for (auto &thread : threads_) {
      thread.join();
    }
  } else {
    shutdown_ = true;
  }
}

std::future<void> ThreadPool::execute(std::function<bool()> func) {
  if (threads_.size() > 1) {
    {
      // Acquire the lock.
      std::lock_guard<std::mutex> lock(mutex_);

      // Get a new completion promise.
      workDone_ = std::promise<void>();

      if (std::accumulate(active_.begin(), active_.end(), 0) != 0) {
        C8_THROW("ThreadPool::execute called before previous execution completed");
      }
      if (shutdown_) {
        C8_THROW("ThreadPool::execute called during shutdown");
      }
      taskFunc_ = func;
      std::fill(active_.begin(), active_.end(), 1);
    }
    cv_.notify_all();
  } else {
    // Get a new completion promise.
    workDone_ = std::promise<void>();
    taskFunc_ = func;

    while (taskFunc_()) {
      // Continue to execute until out of work.
    }

    // Mark that work is complete.
    workDone_.set_value();
  }

  return workDone_.get_future();
}

}  // namespace c8
