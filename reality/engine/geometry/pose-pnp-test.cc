// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":pose-pnp",
    ":homography-decomp",
    "//c8:parameter-data",
    "//c8/geometry:egomotion",
    "//c8/geometry:homography",
    "//c8/geometry:worlds",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xbd7fbda3);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/homography.h"
#include "c8/geometry/worlds.h"
#include "c8/parameter-data.h"
#include "reality/engine/geometry/homography-decomp.h"
#include "reality/engine/geometry/pose-pnp.h"
#include "c8/stats/scope-timer.h"

using testing::Eq;
using testing::FloatNear;
using testing::Pointwise;

namespace c8 {

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsMatrix(const HMatrix &matrix, float tol = 5e-4) {
  return Pointwise(AreWithin(tol), matrix.data());
}

decltype(auto) equalsPoint(const HPoint3 &pt) { return Pointwise(AreWithin(0.0001), pt.data()); }

HMatrix decomposeHomography(const HMatrix &H, HVector3 norm) {
  auto bestNorm = 0.0f;
  HMatrix bestMat = HMatrixGen::i();
  for (const auto &d : decomposeHomographyMat(H)) {
    float dot = d.n.dot(norm);
    if (dot > bestNorm) {
      bestNorm = dot;
      bestMat = cameraMotion(d.t, d.r);
    }
  }
  return bestMat;
}

class PosePnpTest : public ::testing::Test {};

TEST_F(PosePnpTest, TestSolveSmallMapDescendFromSolution) {
  SolvePnPParams params;
  auto p3 = Worlds::axisAlignedPolygonsWorld();
  auto cam = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto p2 = flatten<2>(cam.inv() * p3);
  ScopeTimer rt("test");
  RobustPoseScratchSpace scratch;
  HMatrix reconstructionCam = cam;
  Vector<uint8_t> inliers;

  EXPECT_TRUE(solvePnP(p3, p2, params, &reconstructionCam, &inliers, &scratch));
  EXPECT_THAT(reconstructionCam.data(), equalsMatrix(cam));
}

TEST_F(PosePnpTest, TestSolveSmallMapDescendFromNearSolution) {
  SolvePnPParams params;
  auto p3 = Worlds::axisAlignedPolygonsWorld();
  auto cam1 = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto cam2 = updateWorldPosition(
    cam1, cameraMotion(HPoint3(0.1f, 0.0f, 0.0f), Quaternion(1.0f, 0.0f, 0.0f, 0.0f)));
  auto p2 = flatten<2>(cam2.inv() * p3);
  ScopeTimer rt("test");
  RobustPoseScratchSpace scratch;
  HMatrix reconstructionCam = cam1;
  Vector<uint8_t> inliers;

  EXPECT_TRUE(solvePnP(p3, p2, params, &reconstructionCam, &inliers, &scratch));
  EXPECT_THAT(reconstructionCam.data(), equalsMatrix(cam2));
}

TEST_F(PosePnpTest, TestSolveSmallMapTranslationDescendFromNearIdentity) {
  SolvePnPParams params;
  Vector<HPoint3> p3 = Worlds::flatPlaneFromOriginWorld();
  auto cam1 = HMatrixGen::i();
  auto cam2 = updateWorldPosition(
    cam1, cameraMotion(HPoint3(0.01f, 0.1f, 0.0f), Quaternion(1.0f, 0.0f, 0.0f, 0.0f)));
  auto p2 = flatten<2>(cam2.inv() * p3);
  ScopeTimer rt("test");
  RobustPoseScratchSpace scratch;
  HMatrix reconstructionCam = cam1;
  Vector<uint8_t> inliers;

  EXPECT_TRUE(solvePnP(p3, p2, params, &reconstructionCam, &inliers, &scratch));
  EXPECT_THAT(reconstructionCam.data(), equalsMatrix(cam2));
}

TEST_F(PosePnpTest, TestSolveSmallMapFullMotionDescendFromNearIdentity) {
  SolvePnPParams params;
  Vector<HPoint3> p3 = Worlds::flatPlaneFromOriginWorld();
  auto cam1 = HMatrixGen::i();
  auto cam2 =
    cameraMotion(HPoint3(0.01f, -0.01f, -0.02f), Quaternion(0.997196f, .02f, -.04f, .06f));
  auto p2 = flatten<2>(cam2.inv() * p3);
  ScopeTimer rt("test");
  RobustPoseScratchSpace scratch;
  HMatrix reconstructionCam = cam1;
  Vector<uint8_t> inliers;

  EXPECT_TRUE(solvePnP(p3, p2, params, &reconstructionCam, &inliers, &scratch));
  EXPECT_THAT(reconstructionCam.data(), equalsMatrix(cam2));
}

TEST_F(PosePnpTest, TestSmallMap) {
  RobustPnPParams params;
  params.minZ = 0;
  params.maxZ = 0;
  params.maxOutOfPlaneRotationDegrees = 0;
  auto p3 = Worlds::axisAlignedPolygonsWorld();
  auto cam = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto p2 = flatten<2>(cam.inv() * p3);
  Vector<uint8_t> inliers;
  ScopeTimer rt("test");
  RandomNumbers r;
  RobustPoseScratchSpace scratch;
  HMatrix reconstructionCam = HMatrixGen::i();

  EXPECT_TRUE(
    robustPnP(p3, p2, HMatrixGen::i(), params, &reconstructionCam, &inliers, &r, &scratch));
  EXPECT_THAT(cam.data(), equalsMatrix(reconstructionCam));
}

TEST_F(PosePnpTest, TestPlane) {
  RobustPnPParams params;
  params.minZ = 0;
  params.maxZ = 0;
  params.maxOutOfPlaneRotationDegrees = 0;
  auto p3 = Worlds::axisAlignedGridWorld();
  auto cam = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto p2 = flatten<2>(cam.inv() * p3);
  Vector<uint8_t> inliers;
  ScopeTimer rt("test");
  RandomNumbers r;
  RobustPoseScratchSpace scratch;
  HMatrix reconstructionCam = HMatrixGen::i();

  EXPECT_TRUE(
    robustPnP(p3, p2, HMatrixGen::i(), params, &reconstructionCam, &inliers, &r, &scratch));
  EXPECT_THAT(cam.data(), equalsMatrix(reconstructionCam));
}

TEST_F(PosePnpTest, TestBigMap) {
  RobustPnPParams params;
  params.minZ = 0;
  params.maxZ = 0;
  params.maxOutOfPlaneRotationDegrees = 0;
  auto p3 = Worlds::gravityNormalPlanesWorld();
  auto cam = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto p2 = flatten<2>(cam.inv() * p3);
  Vector<uint8_t> inliers;
  ScopeTimer rt("test");
  RandomNumbers r;
  RobustPoseScratchSpace scratch;
  HMatrix reconstructionCam = HMatrixGen::i();

  EXPECT_TRUE(
    robustPnP(p3, p2, HMatrixGen::i(), params, &reconstructionCam, &inliers, &r, &scratch));
  EXPECT_THAT(cam.data(), equalsMatrix(reconstructionCam));
}

TEST_F(PosePnpTest, TestTargetVsWorldSpace) {
  RobustPnPParams params;
  params.minZ = 0;
  params.maxZ = 0;
  params.maxOutOfPlaneRotationDegrees = 0;
  auto p3 = Worlds::gravityNormalPlanesWorld();
  auto cam = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto p2 = flatten<2>(cam.inv() * p3);
  auto p3InCam = cam.inv() * p3;
  Vector<uint8_t> inliers;
  ScopeTimer rt("test");
  RandomNumbers r;
  RobustPoseScratchSpace scratch;
  HMatrix reconstructionCamImageSpace = HMatrixGen::i();
  HMatrix reconstructionCamWorldSpace = HMatrixGen::i();

  robustPnP(
    p3, p2, HMatrixGen::i(), params, &reconstructionCamWorldSpace, &inliers, &r, &scratch);
  robustPnP(p3InCam, p2, cam, params, &reconstructionCamImageSpace, &inliers, &r, &scratch);
  EXPECT_THAT(reconstructionCamWorldSpace.data(), equalsMatrix(cam));
  EXPECT_THAT((cam * reconstructionCamImageSpace).data(), equalsMatrix(cam));
}

TEST_F(PosePnpTest, TestImageTarget) {
  // Create an artificial image. This would be
  Vector<HPoint3> p3 = {
    {-.75f, 1.0f, 1.0f},
    {.75f, 1.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {.75f, 1.0f, 1.0f},
    {-.75f, -1.0f, 1.0f},
  };
  auto prevCam = cameraMotion(
    HPoint3{0.00999999f, -0.02f, -0.0300001f},
    Quaternion{0.994598f, 0.0219228f, 0.05061f, 0.0879329f});
  auto newCam = cameraMotion(
    HPoint3{0.0201122f, -0.0368589f, -0.0618358f},
    Quaternion{0.978452f, 0.0436085f, 0.100673f, 0.174916f});

  auto imPts = flatten<2>(newCam.inv() * p3);
  for (int i = 0; i < imPts.size(); ++i) {
    // really flatten.
    imPts[i] = HPoint2{imPts[i].x(), imPts[i].y()};
  }
  auto H =
    homographyForPlane(
      prevCam, getScaledPlaneNormal({0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}))
      .inv();

  auto projImPts = flatten<2>(H * extrude<3>(imPts));
  for (int i = 0; i < projImPts.size(); ++i) {
    projImPts[i] = HPoint2{projImPts[i].x(), projImPts[i].y()};
  }

  Vector<uint8_t> inliers;
  ScopeTimer rt("test");
  RobustPoseScratchSpace scratch;
  HMatrix projDelta = HMatrixGen::i();
  SolvePnPParams p;
  p.minPointsForRobustPose = 5;
  p.minPoseInliers = 5;
  EXPECT_TRUE(solvePnP(p3, projImPts, p, &projDelta, &inliers, &scratch));

  auto HP =
    homographyForPlane(
      projDelta, getScaledPlaneNormal({0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}))
      .inv();

  auto FH = HP * H;
  auto fProjImPts = flatten<2>(FH * extrude<3>(imPts));

  for (int i = 0; i < fProjImPts.size(); ++i) {
    EXPECT_NEAR(fProjImPts[i].x(), p3[i].x(), 8e-4);
    EXPECT_NEAR(fProjImPts[i].y(), p3[i].y(), 8e-4);
  }

  auto estCam = decomposeHomography(FH.inv(), HVector3{0.0f, 0.0f, 1.0f}).inv();
  EXPECT_THAT(estCam.data(), equalsMatrix(newCam, 3e-3));

  auto estH =
    homographyForPlane(
      estCam, getScaledPlaneNormal({0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}))
      .inv();
  auto estProjImPts = flatten<2>(estH * extrude<3>(imPts));

  for (int i = 0; i < fProjImPts.size(); ++i) {
    EXPECT_NEAR(estProjImPts[i].x(), p3[i].x(), 2e-3);
    EXPECT_NEAR(estProjImPts[i].y(), p3[i].y(), 2e-3);
  }
}

TEST_F(PosePnpTest, TestImageTargetSolveHomography) {
  // Create an artificial image. This would be
  Vector<HPoint3> p3 = {
    {-.75f, 1.0f, 1.0f},
    {.75f, 1.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {.75f, 1.0f, 1.0f},
    {-.75f, -1.0f, 1.0f},
  };
  auto newCam = cameraMotion(
    HPoint3{0.0201122f, -0.0368589f, -0.0618358f},
    Quaternion{0.978452f, 0.0436085f, 0.100673f, 0.174916f});

  auto imTargetPts = flatten<2>(p3);
  auto camPts = flatten<2>(newCam.inv() * p3);

  Vector<uint8_t> inliers;
  ScopeTimer rt("test");
  RobustPoseScratchSpace scratch;
  HMatrix estCam = HMatrixGen::i();
  SolvePnPParams p;
  p.minPointsForRobustPose = 5;
  p.minPoseInliers = 5;

  EXPECT_TRUE(
    solveImageTargetHomography(imTargetPts, camPts, {}, p, &estCam, &inliers, &scratch));

  EXPECT_THAT(estCam.data(), equalsMatrix(newCam, 2e-7));
}

}  // namespace c8
