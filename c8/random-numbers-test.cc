// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":random-numbers",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x14803036);

#include "c8/random-numbers.h"
#include "gtest/gtest.h"

namespace c8 {

class RandomNumbersTestTest : public ::testing::Test {};

TEST_F(RandomNumbersTestTest, SubsequentRNG) {
  RandomNumbers rng;
  // Subsequent calls should return different values with very high probability.
  EXPECT_NE(rng.nextRandomNormal32f(), rng.nextRandomNormal32f());
}

TEST_F(RandomNumbersTestTest, MultipleRNG) {
  RandomNumbers rng1;
  RandomNumbers rng2;
  RandomNumbers rng3 = {1234};
  bool allSame2 = true;
  bool allSame3 = false;
  for (int i = 0; i < 1000; ++i) {
    auto rng1Val = rng1.nextRandomNormal32f();
    // Two RNG's with the same seed should should return the same values.
    allSame2 &= rng1Val == rng2.nextRandomNormal32f();
    // Two RNG's with different seeds should should return different values.
    allSame3 &= rng1Val == rng3.nextRandomNormal32f();
  }
  EXPECT_TRUE(allSame2);
  EXPECT_FALSE(allSame3);
}

TEST_F(RandomNumbersTestTest, MultinomialDistributionTest) {
  RandomNumbers rng = {1234};

  std::vector<float> weights1 = {0.f, 0.f, 1.f};
  MultinomialDistribution dist(weights1, &rng);

  // Only index 2 has any weight, so it should always be sampled.
  EXPECT_EQ(dist.sample(), 2);
  EXPECT_EQ(dist.sample(), 2);
  EXPECT_EQ(dist.sample(), 2);

  std::vector<float> weights2 = {1.f, 2.f, 3.f};
  dist.reset(weights2);

  std::vector<int> counts(3, 0);
  for (int i = 0; i < 1000; ++i) {
    counts[dist.sample()]++;
  }

  // The counts should be roughly proportional to the weights.
  EXPECT_NEAR(counts[0], 1000 / 6, 100);
  EXPECT_NEAR(counts[1], 1000 / 3, 100);
  EXPECT_NEAR(counts[2], 1000 / 2, 100);
}

}  // namespace c8
