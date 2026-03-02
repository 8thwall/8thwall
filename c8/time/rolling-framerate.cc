// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "rolling-framerate.h",
  };
  deps = {
    ":now",
  };
}
cc_end(0x28566985);

#include "c8/time/rolling-framerate.h"

#include "c8/time/now.h"

namespace c8 {

int64_t RollingFramerate::bufferedMicros() const {
  return frameTimes_.size() <= 1 ? 1 : frameTimes_.back() - frameTimes_.front();
}

double RollingFramerate::fps() const { return (frameTimes_.size() - 1) * 1e6 / bufferedMicros(); }

void RollingFramerate::push() {
  if (lastSuspend_ > 0) {
    return;
  }
  while (bufferedMicros() > bufferMicros_) {
    frameTimes_.pop_front();
  }
  frameTimes_.push_back(nowMicros() - creditMicros_);
}

void RollingFramerate::suspend() {
  if (lastSuspend_ > 0) {
    return;
  }
  lastSuspend_ = nowMicros();
}

void RollingFramerate::resume() {
  if (lastSuspend_ == 0) {
    return;
  }
  creditMicros_ += nowMicros() - lastSuspend_;
  lastSuspend_ = 0;
}

void RollingFramerate::clear() {
  frameTimes_.clear();
  creditMicros_ = 0;
}

}  // namespace c8
