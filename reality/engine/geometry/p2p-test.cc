// Copyright (c) 2024 Niantic, Inc.
// Original Author: Haomin Zhu (hzhu@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":p2p",
    "//c8/geometry:worlds",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xf269408b);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/geometry/worlds.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/geometry/p2p.h"

namespace c8 {

constexpr float kFloatPrecision = 1e-4;

class P2PTest : public ::testing::Test {};

MATCHER_P(AreWithin, eps, "") {
  return std::fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps;
}

decltype(auto) equalsMatrix(const HMatrix &matrix) {
  return testing::Pointwise(AreWithin(kFloatPrecision), matrix.data());
}

TEST_F(P2PTest, P2PImpl) {
  // 15 random points within 5 meters range
  Vector<HPoint3> worldPts = {
    {-2.52459f, -3.73326f, 4.27701f},
    {0.131364f, -1.59107f, -2.48508f},
    {-2.88106f, 0.440123f, -4.67235f},
    {-3.20616f, -1.98969f, 2.50144f},
    {4.53962f, -4.63717f, -2.52216f},
    {-2.073f, -2.07367f, -1.02294f},
    {2.24125f, 0.430682f, 2.31718f},
    {-3.22768f, -2.77897f, -1.81834f},
    {4.4545f, 0.229087f, 4.88296f},
    {0.775774f, 0.21617f, 1.64333f},
    {-3.68741f, 2.77318f, -0.954927f},
    {1.87493f, 3.91644f, -2.89772f},
    {-3.56818f, 4.21889f, 3.77425f},
    {4.19167f, -0.861149f, -4.27085f},
    {-0.538803f, -0.45406f, -4.80195f}};

  auto camToTracking = HMatrixGen::translation(0.3, 0.2, 0.2) * HMatrixGen::rotationD(0, 5, 0);
  auto trackingToWorld = HMatrixGen::translation(-0.3, -0.2, 0.2) * HMatrixGen::rotationD(0, 10, 0);
  auto camToWorld = trackingToWorld * camToTracking;
  auto worldToCam = camToWorld.inv();
  auto camRays = flatten<2>(worldToCam * worldPts);
  ScopeTimer rt("test");

  HMatrix worldToCam1 = HMatrixGen::i();
  HMatrix worldToCam2 = HMatrixGen::i();
  for (int i = 0; i < worldPts.size() - 1; ++i) {
    for (int j = i + 1; j < worldPts.size(); ++j) {
      worldToCam1 = HMatrixGen::i();
      worldToCam2 = HMatrixGen::i();

      bool ableToSolve = p2pImpl(
        worldPts[i],
        worldPts[j],
        camRays[i],
        camRays[j],
        camToTracking,
        &worldToCam1,
        &worldToCam2);
      bool possibleToSolve = std::fabs(camRays[i].x() - camRays[j].x()) > kFloatPrecision
        && std::fabs(camRays[i].y() - camRays[j].y()) > kFloatPrecision;
      if (!possibleToSolve) {
        EXPECT_FALSE(ableToSolve);
      } else {
        EXPECT_TRUE(ableToSolve);
        EXPECT_THAT(
          worldToCam.data(), testing::AnyOf(equalsMatrix(worldToCam1), equalsMatrix(worldToCam2)));
      }
    }
  }
}
}  // namespace c8
