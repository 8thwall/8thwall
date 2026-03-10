// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// A thread pool for running a supplied callback on a set of threads.

#pragma once

#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>

#include "c8/vector.h"

namespace c8 {

class ThreadPool {
public:
  // Construct a ThreadPool which will initialize a set of nthreads.
  ThreadPool(int nthreads);

  // Destroy the ThreadPool, leaving any new work unfinished and joining the threads.
  ~ThreadPool();

  // Disallow move.
  ThreadPool(ThreadPool &&) = delete;
  ThreadPool &operator=(ThreadPool &&) = delete;

  // Disallow copying.
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

  // Call method to begin processing func on all available threads. If func returns false, a thread
  // in the pool will become idle. Returns a std::future which can be used for waiting until all
  // work in the pool is complete.
  std::future<void> execute(std::function<bool()> func);

private:
  std::function<bool()> taskFunc_;
  Vector<std::thread> threads_;
  bool shutdown_;
  Vector<int> active_;
  std::promise<void> workDone_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

}  // namespace c8
