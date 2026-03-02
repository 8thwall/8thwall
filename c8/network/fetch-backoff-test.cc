// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Paris Morgan (parismorgan@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":fetch-backoff",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x1f3552e6);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/network/fetch-backoff.h"

namespace c8 {

class FetchBackoffTest : public ::testing::Test {};

TEST_F(FetchBackoffTest, MaxInterval) {
  RandomNumbers random = {std::random_device()};
  int64_t interval = 1 * 1000;
  int64_t maxInterval = 1 * 1000;
  FetchBackoff b(interval, maxInterval, &random);
  EXPECT_EQ(b.intervalMillis(), interval);
  b.increaseInterval();
  EXPECT_EQ(b.intervalMillis(), interval);
}

TEST_F(FetchBackoffTest, MaxIntervalLessThanInterval) {
  RandomNumbers random = {std::random_device()};
  int64_t interval = 10 * 1000;
  int64_t maxInterval = 1 * 1000;
  FetchBackoff b(interval, maxInterval, &random);
  EXPECT_EQ(b.intervalMillis(), maxInterval);
  b.increaseInterval();
  EXPECT_EQ(b.intervalMillis(), maxInterval);
}

TEST_F(FetchBackoffTest, IncreaseInterval) {
  for (int i = 0; i < 100; ++i) {
    RandomNumbers random = {std::random_device()};
    int64_t interval = i * 1000;
    int64_t maxInterval = i * 10 * 1000;
    FetchBackoff b(interval, maxInterval, &random);
    EXPECT_EQ(b.intervalMillis(), interval);

    b.increaseInterval();
    EXPECT_LE(b.intervalMillis(), interval * 2 + 1 * 1000);
    EXPECT_LE(b.intervalMillis(), maxInterval);

    b.increaseInterval();
    EXPECT_LE(b.intervalMillis(), interval * 4 + 1 * 1000);
    EXPECT_LE(b.intervalMillis(), maxInterval);

    b.increaseInterval();
    EXPECT_LE(b.intervalMillis(), interval * 8 + 1 * 1000);
    EXPECT_LE(b.intervalMillis(), maxInterval);

    b.increaseInterval();
    EXPECT_LE(b.intervalMillis(), interval * 16 + 1 * 1000);
    EXPECT_LE(b.intervalMillis(), maxInterval);
  }
}

TEST_F(FetchBackoffTest, TwoObjects) {
  for (int i = 1; i < 100; ++i) {
    // Don't use random_device here since then there is always a non 0 probability that we can get a
    // collision where the two objects have the same `increaseInterval()`.
    RandomNumbers random;
    int64_t interval = i * 1000;
    int64_t maxInterval = i * 10 * 1000;
    FetchBackoff b1(interval, maxInterval, &random);
    FetchBackoff b2(interval, maxInterval, &random);
    // Interval = i, so both will be interval.
    EXPECT_EQ(b1.intervalMillis(), b2.intervalMillis());
    EXPECT_EQ(b1.intervalMillis(), interval);

    // Interval = 2 * i + noise.
    b1.increaseInterval();
    b2.increaseInterval();
    EXPECT_NE(b1.intervalMillis(), b2.intervalMillis());

    // Interval = 4 * i + noise.
    b1.increaseInterval();
    b2.increaseInterval();
    EXPECT_NE(b1.intervalMillis(), b2.intervalMillis());

    // Interval = 8 * i + noise.
    b1.increaseInterval();
    b2.increaseInterval();
    EXPECT_NE(b1.intervalMillis(), b2.intervalMillis());

    // Interval = 16 * i + noise, which is bigger than maxInterval (10), so both will be
    // maxInterval.
    b1.increaseInterval();
    b2.increaseInterval();
    EXPECT_EQ(b1.intervalMillis(), b2.intervalMillis());
    EXPECT_EQ(b1.intervalMillis(), maxInterval);
  }
}

TEST_F(FetchBackoffTest, Factory) {
  int64_t maxInterval = 10 * 1000;
  auto b = FetchBackoff::every1s(maxInterval);
  EXPECT_EQ(b.intervalMillis(), 1 * 1000);
  b.increaseInterval();
  EXPECT_GE(b.intervalMillis(), 2 * 1000);
  EXPECT_LE(b.intervalMillis(), maxInterval);
  b.increaseInterval();
  EXPECT_GE(b.intervalMillis(), 4 * 1000);
  EXPECT_LE(b.intervalMillis(), maxInterval);
  b.increaseInterval();
  EXPECT_GE(b.intervalMillis(), 8 * 1000);
  EXPECT_LE(b.intervalMillis(), maxInterval);
}

}  // namespace c8
