// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <cstdint>
#include <limits>

namespace c8 {

// Get the current time in the highest available resolution. This is the preferred method for
// performance evaluation by computing deltas across subsequent calls, but many platforms will not
// return epoch time in this case.
int64_t nowMicros();

// Zero out your int64_t time so the first timestamp is zero, all other timestamps is time from the
// fist timestamp. This avoids the issue with converting int64_t to float when the value is large.
class TimeReset {
public:
  float shift(int64_t timestamp);

private:
  int64_t firstTimestamp_ = std::numeric_limits<int64_t>::max();
};

}  // namespace c8
