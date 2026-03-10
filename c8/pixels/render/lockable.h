// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#pragma once

#include <memory>
#include <mutex>

namespace c8 {

template <class T>
class Lockable {
public:
  Lockable();
  Lockable(std::unique_ptr<T> &&c) : class_(std::move(c)){};

  T *operator->() {
    return class_.get();
  }

  T *ptr() {
    return class_.get();
  }

  T &ref() {
    return *class_;
  }

  std::lock_guard<std::mutex> lock() {
    return std::lock_guard<std::mutex>(mtx_);
  }

private:
  std::mutex mtx_;
  std::unique_ptr<T> class_;
};
}  // namespace c8
