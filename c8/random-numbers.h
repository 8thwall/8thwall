// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// Holds state for repeated random number generation.

#pragma once

#include <algorithm>
#include <random>

namespace c8 {

class RandomNumbers {
public:
  // Default constructor.
  RandomNumbers() = default;
  RandomNumbers(std::random_device rd) : rng(rd()) {}
  RandomNumbers(uint32_t seed) : rng(seed) {}

  // Default move constructors.
  RandomNumbers(RandomNumbers &&) = default;
  RandomNumbers &operator=(RandomNumbers &&) = default;

  // Random number functions.
  inline float nextRandomNormal32f() { return gaussian32f(rng); }
  inline float nextUniform32f() { return uniform32f(rng); }
  inline uint32_t nextUnsignedInt() { return uniform32i(rng); }
  inline uint64_t nextUnsignedInt64() { return uniform64i(rng); }

  inline int32_t nextUniformInt(int aInc, int bEx) {
    return static_cast<int32_t>(nextUnsignedInt() % (bEx - aInc)) + aInc;
  }

  template <class RandomIt>
  void shuffle(RandomIt first, RandomIt last) {
    std::shuffle(first, last, rng);
  }

  // Disallow copying.
  RandomNumbers(const RandomNumbers &) = delete;
  RandomNumbers &operator=(const RandomNumbers &) = delete;

private:
  std::mt19937 rng;
  std::normal_distribution<float> gaussian32f;
  std::uniform_real_distribution<float> uniform32f;
  std::uniform_int_distribution<uint32_t> uniform32i;
  std::uniform_int_distribution<uint64_t> uniform64i;
};

class MultinomialDistribution {
public:
  MultinomialDistribution(const std::vector<float> &weights, RandomNumbers *rng) : rng_(rng) {
    reset(weights);
  }

  void reset(const std::vector<float> &weights) {
    float totalSamplingWeight = 0.0f;
    accumulatedWeights_.resize(weights.size(), 0.0f);

    // Compute the accumulated weights and the total sampling weight.
    for (size_t i = 0; i < weights.size(); ++i) {
      totalSamplingWeight += weights[i];
      accumulatedWeights_[i] = totalSamplingWeight;
    }
    // Normalize the accumulated weights so the last one is 1.0.
    for (auto &weight : accumulatedWeights_) {
      weight /= totalSamplingWeight;
    }
  }

  size_t sample() {
    // Binary search to find the index of the sampled weight.
    auto sampleIt = std::lower_bound(
      accumulatedWeights_.begin(), accumulatedWeights_.end(), rng_->nextUniform32f());
    // Return the index of the sampled weight.
    return std::distance(accumulatedWeights_.begin(), sampleIt);
  }

private:
  RandomNumbers *rng_;
  std::vector<float> accumulatedWeights_;
};

}  // namespace c8
