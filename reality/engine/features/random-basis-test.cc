// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":random-basis",
    "//c8:float-vector",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xe169a316);

#include "reality/engine/features/random-basis.h"

#include <random>

#include "c8/float-vector.h"

#include <gtest/gtest.h>

namespace c8 {

class RandomBasisTest : public ::testing::Test {};

TEST_F(RandomBasisTest, GenerateAndProjectBasis) {
  std::mt19937 rnd(0);
  std::normal_distribution dist(0.0f, 1.0f);
  Vector<FloatVector> data;
  constexpr int inputSize = 32;
  constexpr int numVectors = 25;

  // Generate 10 vectors with 50-dimensions, using a normal distribution.
  auto genRnd = [&rnd, &dist]() { return dist(rnd); };
  for (int i = 0; i < numVectors; ++i) {
    FloatVector vec(inputSize);
    std::generate(vec.begin(), vec.end(), genRnd);
    // Input must be normalized.
    vec.l2Normalize();
    data.emplace_back(std::move(vec));
  }

  using Descriptor = ImageDescriptor<32>;

  RandomBasis<Descriptor> basis(inputSize);

  Vector<Descriptor> projections;
  for (int i = 0; i < numVectors; ++i) {
    projections.emplace_back(basis.projectAndBinarize(data[i]));
  }

  std::uniform_int_distribution<> distI(0, data.size() - 1);

  int matches = 0;
  int comparisons = 0;

  for (int i = 0; i < 100; ++i) {
    int q = distI(rnd);
    int a = distI(rnd);
    int b = distI(rnd);

    const FloatVector &vecQ = data[q];
    const FloatVector &vecA = data[a];
    const FloatVector &vecB = data[b];

    float distA = 1.0f - innerProduct(vecQ, vecA);
    float distB = 1.0f - innerProduct(vecQ, vecB);

    float hammingA = projections[q].hammingDistance(projections[a]);
    float hammingB = projections[q].hammingDistance(projections[b]);

    if ((distA - distB) * (hammingA - hammingB) >= 0) {
      matches++;
    }
    comparisons++;
  }

  // Expect at least 90% of comparisons to match for this 256-bit projection.
  EXPECT_LT(0.9f, static_cast<float>(matches) / comparisons);
}

}  // namespace c8
