// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "now.h",
  };
  deps = {};
}
cc_end(0xc92cf788);

#include "c8/time/now.h"

#ifdef JAVASCRIPT
#include <emscripten/emscripten.h>
#else
#include <chrono>
#endif

namespace c8 {
int64_t nowMicros() {
#ifdef JAVASCRIPT
  return static_cast<int64_t>(emscripten_get_now() * 1e3);
#else
  return std::chrono::time_point_cast<std::chrono::microseconds>(
           std::chrono::high_resolution_clock::now())
    .time_since_epoch()
    .count();
#endif
}

float TimeReset::shift(int64_t timestamp) {
  if (firstTimestamp_ == std::numeric_limits<int64_t>::max()) {
    firstTimestamp_ = timestamp;
  }
  return static_cast<float>(timestamp - firstTimestamp_);
}

}  // namespace c8
