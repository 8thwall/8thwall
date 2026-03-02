// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Paris Morgan (parismorgan@nianticlabs.com)
//
// Holds a class which can be used for exponential back-off of network requests.

#pragma once

#include "c8/random-numbers.h"

namespace c8 {

class FetchBackoff {
public:
  static FetchBackoff every1s(int64_t maxIntervalMillis);

  /**
   * @param intervalMillis the starting interval in microseconds. Exponential doubling uses this as
   * the starting value.
   * @param maxIntervalMillis the max interval in microseconds.
   * @param random the random numbers used to add noise to the interval.
   */
  FetchBackoff(int64_t intervalMillis, int64_t maxIntervalMillis, RandomNumbers *random)
      : baseIntervalMillis_(std::min(intervalMillis, maxIntervalMillis)),
        intervalMillis_(std::min(intervalMillis, maxIntervalMillis)),
        maxIntervalMillis_(maxIntervalMillis),
        random_(random){};

  // Increase the interval. We:
  //   1) Double the baseIntervalMillis.
  //   2) Add (0, 1] seconds to the baseIntervalMillis.
  //   3) Store this in intervalMillis_ to use as the interval.
  void increaseInterval();
  int64_t intervalMillis() const { return intervalMillis_; }

private:
  // The current interval without random wait time added.
  int64_t baseIntervalMillis_ = 0;
  // The current interval with random wait time added.
  int64_t intervalMillis_ = 0;
  // The max interval.
  int64_t maxIntervalMillis_ = 0;

  RandomNumbers *random_;
};

}  // namespace c8
