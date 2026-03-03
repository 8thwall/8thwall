// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Thread-safe task queue.

#pragma once

#include <functional>

#include "c8/vector.h"

namespace c8 {

class ThreadPool;

class TaskQueue {
public:
  // Default constructor.
  TaskQueue() noexcept = default;

  // Default move constructors.
  TaskQueue(TaskQueue &&) = default;
  TaskQueue &operator=(TaskQueue &&) = default;

  // Disallow copying.
  TaskQueue(const TaskQueue &) = delete;
  TaskQueue &operator=(const TaskQueue &) = delete;

  // Add a new task to the TaskQueue.
  virtual void addTask(std::function<void()> func) { tasks_.push_back(std::move(func)); }

  // Pop and execute all of the tasks in the queue, synchronously.
  void execute();

  // Pop and execute all of the tasks in the queue in parallel, blocking until all are complete.
  void executeWithThreadPool(ThreadPool *pool);

private:
  Vector<std::function<void()>> tasks_;
};

}  // namespace c8
