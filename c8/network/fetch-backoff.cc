// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Paris Morgan (parismorgan@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"fetch-backoff.h"};
  deps = {
    "//c8:random-numbers",
  };
}
cc_end(0xe62feebc);

#include "c8/network/fetch-backoff.h"

namespace c8 {

FetchBackoff FetchBackoff::every1s(int64_t maxInterval) {
  static RandomNumbers random = {std::random_device()};
  return {1 * 1000, maxInterval, &random};
}

void FetchBackoff::increaseInterval() {
  if (baseIntervalMillis_ < maxIntervalMillis_) {
    baseIntervalMillis_ *= 2;
    intervalMillis_ = std::min(
      maxIntervalMillis_,
      baseIntervalMillis_ + static_cast<int64_t>(random_->nextUniform32f() * 1000));
  }
}

}  // namespace c8
