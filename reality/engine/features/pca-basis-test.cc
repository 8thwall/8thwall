// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":pca-basis",
    "//c8:float-vector",
    "//c8/io:capnp-messages",
    "@com_google_googletest//:gtest_main",
    "@eigen3",
  };
}
cc_end(0x1d0831e5);

#include "reality/engine/features/pca-basis.h"

#include <Eigen/Dense>
#include <random>

#include "c8/float-vector.h"
#include "c8/io/capnp-messages.h"

#include "gtest/gtest.h"

namespace std {

// Give capnproto iterators STL-iterator support.
template <typename Container, typename Element>
struct iterator_traits<capnp::_::IndexingIterator<Container, Element>>
    : public std::iterator<std::random_access_iterator_tag, Element, int> {};

}  // namespace std

namespace c8 {

class PcaBasisTest : public ::testing::Test {};

TEST_F(PcaBasisTest, GenerateBasis) {
  Vector<FloatVector> data;
  data.emplace_back(FloatVector{1, 2, -1});
  data.emplace_back(FloatVector{2, 4, 7});
  data.emplace_back(FloatVector{3, 6, 2});
  data.emplace_back(FloatVector{4, 8, -3});
  data.emplace_back(FloatVector{5, 10, 4});

  constexpr float epsilon = 0.00005;

  float retainVariance = 0.99f;
  PcaBasis pca = PcaBasis::generateBasis(data.begin(), data.end(), &retainVariance);

  // Should retain all variance, since the second column is 100% correlated with first column.
  EXPECT_FLOAT_EQ(1.0, retainVariance);

  Vector<FloatVector> projected;

  for (int i = 0; i < data.size(); ++i) {
    // Project each vector into the PCA space and whiten.
    projected.emplace_back(pca.projectAndWhiten(data[i]));

    // Assert two dimensions retained.
    ASSERT_EQ(2, projected.back().size());
  }

  // Expect zero mean in the projected samples.
  FloatVector projectedMean(2);
  for (auto &vec : projected) {
    projectedMean += vec;
  }
  EXPECT_NEAR(0.0, projectedMean[0], epsilon);
  EXPECT_NEAR(0.0, projectedMean[1], epsilon);

  // Copy the projected data into an Eigen matrix.
  Eigen::MatrixXf projectedData(projected[0].size(), projected.size());
  for (int i = 0; i < projected.size(); ++i) {
    for (int j = 0; j < projected[i].size(); ++j) {
      projectedData(j, i) = projected[i][j];
    }
  }

  // Expect a covariance matrix near to identity for these whitened samples.
  Eigen::MatrixXf zeroMean = projectedData.colwise() - projectedData.rowwise().mean();
  float oneOverN = 1.0f / projectedData.cols();
  Eigen::MatrixXf cov = oneOverN * (zeroMean * zeroMean.transpose());
  EXPECT_NEAR(1.0, cov(0, 0), epsilon);
  EXPECT_NEAR(0.0, cov(1, 0), epsilon);
  EXPECT_NEAR(0.0, cov(0, 1), epsilon);
  EXPECT_NEAR(1.0, cov(1, 1), epsilon);
}

TEST_F(PcaBasisTest, LoadStore) {
  Vector<FloatVector> data;
  data.emplace_back(FloatVector{1, 3, -1});
  data.emplace_back(FloatVector{2, 4, 7});
  data.emplace_back(FloatVector{3, 2, 2});
  data.emplace_back(FloatVector{4, 1, -3});
  data.emplace_back(FloatVector{5, 50, 7});
  data.emplace_back(FloatVector{-2, -13, -3});
  data.emplace_back(FloatVector{8, 18, 4});

  float retainVariance = 0.99f;
  PcaBasis pca = pca.generateBasis(data.begin(), data.end(), &retainVariance);

  // Store the basis in a capnp message.
  MutableRootMessage<PcaBasisData> message;
  pca.storeBasis(message.builder());

  // Load the basis in a new PcaBasis class.
  PcaBasis pcaPrime = PcaBasis::loadBasis(message.reader());

  for (int i = 0; i < data.size(); ++i) {
    // Project each vector into each pca space and whiten.
    FloatVector proj = pca.projectAndWhiten(data[i]);
    FloatVector projPrime = pcaPrime.projectAndWhiten(data[i]);

    // The projected vectors should be identical.
    ASSERT_EQ(proj.size(), projPrime.size());
    EXPECT_TRUE(std::equal(proj.begin(), proj.end(), projPrime.begin()));
  }

  // Store basis with fewer dimensions.
  pcaPrime.storeBasis(message.builder(), 2);

  // Load the 1-dim basis PcaBasis class.
  PcaBasis pcaLowDim = PcaBasis::loadBasis(message.reader());

  for (int i = 0; i < data.size(); ++i) {
    // Project each vector into each pca space and whiten.
    FloatVector proj = pca.projectAndWhiten(data[i]);
    FloatVector projLowDim = pcaLowDim.projectAndWhiten(data[i]);

    // We kept 2 dimensions.
    ASSERT_EQ(2, projLowDim.size());

    // The remaining dimensions should be equal.
    EXPECT_TRUE(std::equal(projLowDim.begin(), projLowDim.end(), proj.begin()));
  }
}

TEST_F(PcaBasisTest, BinarizeAndStore) {
  Vector<FloatVector> data;
  const float stddev = 1.0f;
  std::mt19937 rnd(0);
  std::normal_distribution<float> dist{5, stddev};
  auto genRnd = [&rnd, &dist]() { return dist(rnd); };

  for (int i = 0; i < 10; ++i) {
    FloatVector vec(512);
    std::generate(vec.begin(), vec.end(), genRnd);
    data.emplace_back(std::move(vec));
  }

  float retainVariance = 0.99f;
  PcaBasis pca = pca.generateBasis(data.begin(), data.end(), &retainVariance);
  EXPECT_EQ(9, pca.projectionSize());

  // Store the basis in a PcaBasisData message.
  MutableRootMessage<PcaBasisData> pcaMessage;
  pca.storeBasis(pcaMessage.builder());

  // Binarize and store the basis in a BinaryPcaData message.
  MutableRootMessage<BinaryPcaData> binaryMessage;
  pca.storeBinaryBasis(binaryMessage.builder());

  auto pm = pcaMessage.reader();
  auto bm = binaryMessage.reader();

  // Basis vector size should be unchanged in binary message.
  EXPECT_EQ(pm.getBasisVectorSize(), bm.getBasisVectorSize());

  // Whitening components should be unchanged.
  EXPECT_EQ(pm.getWhitening().size(), bm.getWhitening().size());
  EXPECT_TRUE(
    std::equal(pm.getWhitening().begin(), pm.getWhitening().end(), bm.getWhitening().begin()));

  // Quantized translation should be close to actual.
  EXPECT_EQ(pm.getTranslation().size(), bm.getTranslationIdx().size());
  EXPECT_TRUE(std::equal(
    pm.getTranslation().begin(),
    pm.getTranslation().end(),
    bm.getTranslationIdx().begin(),
    [lut = bm.getTranslationLut(), stddev](float lhs, int rhsIdx) {
      return std::abs(lhs - lut[rhsIdx]) < 0.07 * stddev;
    }));

  // Indices should span the LUT range.
  float minEl = *std::min_element(bm.getTranslationIdx().begin(), bm.getTranslationIdx().end());
  float maxEl = *std::max_element(bm.getTranslationIdx().begin(), bm.getTranslationIdx().end());
  EXPECT_EQ(0, minEl);
  EXPECT_EQ(bm.getTranslationLut().size() - 1, maxEl);

  // Basis should be 1/64th size since components are packed as bits in a uint64_t.
  EXPECT_EQ(pm.getBasis().size() / 64, bm.getBasis().size());

  int dimensionality = pm.getBasis().size() / pm.getBasisVectorSize();
  for (int m = 0; m < dimensionality; ++m) {
    for (int n = 0; n < pm.getBasisVectorSize(); ++n) {
      float pmf = pm.getBasis()[n * dimensionality + m];
      uint64_t block = bm.getBasis()[(n + m * bm.getBasisVectorSize()) / 64];
      uint64_t bit = block & (static_cast<uint64_t>(0b1) << (n % 64));

      bool psign = 0.0f < pmf;
      bool bsign = bit != 0;
      EXPECT_EQ(psign, bsign) << m <<  "  " << n;
    }
  }
}

}  // namespace c8
