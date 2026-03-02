// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <deque>

namespace c8 {

class RollingFramerate {
public:
  // Default constructor: 10 seconds buffered.
  RollingFramerate() = default;

  // Set a custom buffer.
  RollingFramerate(int64_t bufferMicros) : bufferMicros_(bufferMicros) {}

  // Default move constructors.
  RollingFramerate(RollingFramerate &&) = default;
  RollingFramerate &operator=(RollingFramerate &&) = default;

  // Disallow copying.
  RollingFramerate(const RollingFramerate &) = delete;
  RollingFramerate &operator=(const RollingFramerate &) = delete;

  int64_t bufferedMicros() const;

  double fps() const;

  void suspend();

  void resume();

  void push();

  void clear();

private:
  std::deque<int64_t> frameTimes_;
  int64_t bufferMicros_ = 10e6;
  int64_t creditMicros_ = 0;
  int64_t lastSuspend_ = 0;
};

}  // namespace c8
