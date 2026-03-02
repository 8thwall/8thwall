// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":epipolar",
    "//c8/geometry:egomotion",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x50e9c2de);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cfloat>

#include "c8/geometry/egomotion.h"
#include "c8/geometry/vectors.h"
#include "reality/engine/geometry/epipolar.h"

using testing::Eq;
using testing::FloatNear;
using testing::Pointwise;

namespace c8 {

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsPoint(const HPoint2 &pt) { return Pointwise(AreWithin(0.0001), pt.data()); }
decltype(auto) equalsVector(float x, float y, float z) {
  return Pointwise(AreWithin(1e-5), HVector3(x, y, z).data());
}

EpipolarDepthResult depth(
  const EpipolarDepthImagePrework &iw, HPoint2 wordsPt, HVector2 dictPt, uint8_t scale = 0) {
  auto pw = epipolarDepthPointPrework(iw, wordsPt);
  return depthOfEpipolarPoint(iw, pw, dictPt, scale);
}

// Testing helper: Computes the portion of `EpipolarDepthPointPrework` that's needed for testing
// closestLineSegmentPoint() in isolation.
EpipolarDepthPointPrework closestLineSegmentPointPrework(std::pair<HVector2, HVector2> segment) {
  auto segmentVector = segment.second - segment.first;
  auto segmentSqLen = segmentVector.dot(segmentVector);
  bool isPoint = segmentSqLen < 1e-10f;
  float norm = isPoint ? 1.0f : 1.0f / segmentSqLen;
  auto normSegmentVector = norm * segmentVector;
  return {
    segment,
    segmentVector,
    isPoint,
    normSegmentVector,
  };
}

// Points along the center, x/y axes, and diagonal.
// The y-point is closer to the center than the x-point.
// The center point is at depth 3, the x/y points are at depth 1 and the diagonal point is at
// depth 2.
const HPoint2 CENTER_PT{0.0f, 0.0f};
const HPoint2 X_PT{0.1f, 0.0f};
const HPoint2 Y_PT{0.0f, 0.05f};
const HPoint2 DIAG_PT{-0.2f, -0.2f};
const HPoint3 CENTER_PT_3{CENTER_PT.x() * 3.0f, CENTER_PT.y() * 3.0f, 3.0f};
const HPoint3 X_PT_3{X_PT.x() * 1.0f, X_PT.y() * 1.0f, 1.0f};
const HPoint3 Y_PT_3{Y_PT.x() * 1.0f, Y_PT.y() * 1.0f, 1.0f};
const HPoint3 DIAG_PT_3{DIAG_PT.x() * 2.0f, DIAG_PT.y() * 2.0f, 2.0f};

const HMatrix IDENTITY = HMatrixGen::i();
const HMatrix TRANSLATE_X = HMatrixGen::translation(0.05f, 0.0f, 0.0f);
const HMatrix TRANSLATE_Z = HMatrixGen::translation(0.0f, 0.0f, -0.05f);
const HMatrix ROTATE_Y = HMatrixGen::rotationD(0.0f, 30.0f, 0.0f);
const HMatrix FULL = updateWorldPosition(ROTATE_Y, updateWorldPosition(TRANSLATE_Z, TRANSLATE_X));

const HPoint2 LEFT_PT{-0.2f, 0.1f};
const HPoint2 MID_PT{0.0f, 0.0f};
const HPoint2 RIGHT_PT{0.2f, -0.1f};

class EpipolarTest : public ::testing::Test {};

TEST_F(EpipolarTest, TestEssentialMatrixForCameras) {
  auto cam1 = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto cam2 = HMatrixGen::translation(7.0, 6.0, 16.0) * HMatrixGen::rotationD(23.0, 199.0, -25.0);

  auto matrix = essentialMatrixForCameras(cam1, cam2);

  // Verify that x'Ex = 0
  auto pt = HPoint3(1.0f, 2.0f, 3.0f);
  auto p1 = (cam1.inv() * pt).flatten().extrude();
  auto p2 = (cam2.inv() * pt).flatten().extrude();

  auto p3 = matrix * p1;                                            // Ex
  float dot = p3.x() * p2.x() + p3.y() * p2.y() + p3.z() * p2.z();  // x'Ex
  EXPECT_NEAR(0.0f, dot, 3e-5);
}

TEST_F(EpipolarTest, TestDistanceFromEpipolar) {
  auto cam1 = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto cam2 = updateWorldPosition(cam1, HMatrixGen::translation(1.0f, 0.0f, 0.0f));
  auto cam3 = updateWorldPosition(cam1, HMatrixGen::translation(0.0f, 1.0f, 0.0f));
  auto cam4 = updateWorldPosition(cam1, HMatrixGen::translation(1.0f, 1.0f, 0.0f));
  auto cam5 = updateWorldPosition(cam1, HMatrixGen::rotationD(11.0f, 9.0f, -10.0f));

  auto pt = HPoint3(1.0f, 2.0f, 3.0f);
  auto p1 = (cam1.inv() * pt).flatten();
  auto p2 = (cam2.inv() * pt).flatten();
  auto p3 = (cam3.inv() * pt).flatten();
  auto p4 = (cam4.inv() * pt).flatten();
  auto p5 = (cam4.inv() * pt).flatten();

  EXPECT_NEAR(0.0f, distanceFromEpipolar(essentialMatrixForCameras(cam1, cam2), p1, p2), 1e-7);
  EXPECT_NEAR(0.0f, distanceFromEpipolar(essentialMatrixForCameras(cam1, cam3), p1, p3), 1e-7);
  EXPECT_NEAR(0.0f, distanceFromEpipolar(essentialMatrixForCameras(cam1, cam4), p1, p4), 1e-7);
  EXPECT_EQ(FLT_MAX, distanceFromEpipolar(essentialMatrixForCameras(cam1, cam5), p1, p5));

  auto p2e = HPoint2(p2.x(), p2.y() + 1.0f);
  auto p3e = HPoint2(p3.x() - 0.5f, p3.y());
  auto p4e = HPoint2(p4.x() - 1.0f, p4.y() + 1.0f);
  auto p5e = HPoint2(p5.x() + 1.0f, p5.y() - 1.0f);

  EXPECT_FLOAT_EQ(1.0f, distanceFromEpipolar(essentialMatrixForCameras(cam1, cam2), p1, p2e));
  EXPECT_FLOAT_EQ(0.5f, distanceFromEpipolar(essentialMatrixForCameras(cam1, cam3), p1, p3e));
  EXPECT_FLOAT_EQ(
    std::sqrt(2.0f), distanceFromEpipolar(essentialMatrixForCameras(cam1, cam4), p1, p4e));
  EXPECT_EQ(FLT_MAX, distanceFromEpipolar(essentialMatrixForCameras(cam1, cam5), p1, p5e));
}

TEST_F(EpipolarTest, TestGroundPointTriangulationOnGround) {
  // If we're on the ground we should have a depth of zero no matter our rotation.
  for (int x = 0; x <= 360; x += 20) {
    for (int y = 0; y <= 360; y += 20) {
      for (int z = 0; z <= 360; z += 20) {
        auto extrinsic = HMatrixGen::translation(0.f, -1.f, 0.f) * HMatrixGen::rotationD(x, y, z);
        auto scaledGroundPlaneNormal = groundPointTriangulationPrework(extrinsic);
        auto depth = 1.f / scaledGroundPlaneNormal.dot(HVector3(0.f, -.1f, 1.f));
        EXPECT_NEAR(depth, 0.f, 2e-6f);
      }
    }
  }
}

TEST_F(EpipolarTest, TestGroundPointTriangulationAimingDownAtGround) {
  // 30-60-90 triangle with the extrinsic 1 unit from ground looking down at the ground. Here we
  // have 60 degrees between the extrinsic and ground.
  auto extrinsic = HMatrixGen::translation(0.f, 0.f, 0.f) * HMatrixGen::rotationD(30.f, 0.f, 0.f);
  auto scaledGroundPlaneNormal = groundPointTriangulationPrework(extrinsic);

  // The ray from the center of the camera should be 2 units from the camera along the hypotenuse.
  auto depth = 1.f / scaledGroundPlaneNormal.dot(HVector3(0.f, 0.f, 1.f));
  EXPECT_FLOAT_EQ(2.f, depth);

  // Other rays moved along x should also be 2 units from the camera along the hypotenuse of
  // other triangles parallel to the original triangle we constructed.
  depth = 1.f / scaledGroundPlaneNormal.dot(HVector3(10.f, 0.f, 1.f));
  EXPECT_FLOAT_EQ(depth, 2.f);

  // If we move our ray +y, we point more to the horizon, so should be farther.
  depth = 1.f / scaledGroundPlaneNormal.dot(HVector3(0.f, .1f, 1.f));
  EXPECT_GT(depth, 2.f);

  // If we move our ray -y, we point more at the ground, so should be closer.
  depth = 1.f / scaledGroundPlaneNormal.dot(HVector3(0.f, -.1f, 1.f));
  EXPECT_LT(depth, 2.f);
}

TEST_F(EpipolarTest, TestGroundPointTriangulationRayInTwoCameras) {
  // Have a ray in world space and two cameras. Solve for ray in each camera's space, calculate
  // depth, and confirm both get the same depth.
  auto extrinsic1 = HMatrixGen::translation(0.f, 0.f, 0.f) * HMatrixGen::rotationD(30.f, 0.f, 0.f);
  auto extrinsic2 = HMatrixGen::translation(0.f, 0.f, 0.f) * HMatrixGen::rotationD(60.f, 0.f, 0.f);
  auto scaledGroundPlaneNormal1 = groundPointTriangulationPrework(extrinsic1);
  auto scaledGroundPlaneNormal2 = groundPointTriangulationPrework(extrinsic2);
  Vector<HVector3> raysInWorld{
    {0.f, -.5f, 0.866f}, {0.f, -0.866f, 0.5f}, {0.f, -.6f, 0.866f}, {0.f, -.5f, 0.766f}};

  for (const auto &rayInWorld : raysInWorld) {
    auto rayInCam1 = extrinsic1.inv() * rayInWorld;
    auto rayInCam2 = extrinsic2.inv() * rayInWorld;

    auto depth1 = 1.f / scaledGroundPlaneNormal1.dot(rayInCam1);
    auto depth2 = 1.f / scaledGroundPlaneNormal2.dot(rayInCam2);
    EXPECT_NEAR(depth1, depth2, 1e-6f);
  }
}

// TODO(paris): Can write tests that verify:
// - Should be able to rotate about world Y with no change
// - Should be able to rotatio about self Z with no change

TEST_F(EpipolarTest, TestEpipolarLineSegmentIdentity) {
  auto w = epipolarDepthImagePrework(IDENTITY, IDENTITY);
  auto [lp1, lp2] = epipolarDepthPointPrework(w, LEFT_PT).lineSegment;
  auto [mp1, mp2] = epipolarDepthPointPrework(w, MID_PT).lineSegment;
  auto [rp1, rp2] = epipolarDepthPointPrework(w, RIGHT_PT).lineSegment;

  // Line segments should have length 0 and should be at the original locations.
  EXPECT_FLOAT_EQ(lp1.x(), -0.2f);
  EXPECT_FLOAT_EQ(lp1.y(), 0.1f);
  EXPECT_FLOAT_EQ(mp1.x(), 0.0f);
  EXPECT_FLOAT_EQ(mp1.y(), 0.0f);
  EXPECT_FLOAT_EQ(rp1.x(), 0.2f);
  EXPECT_FLOAT_EQ(rp1.y(), -0.1f);
  EXPECT_FLOAT_EQ(lp2.x(), -0.2f);
  EXPECT_FLOAT_EQ(lp2.y(), 0.1f);
  EXPECT_FLOAT_EQ(mp2.x(), 0.0f);
  EXPECT_FLOAT_EQ(mp2.y(), 0.0f);
  EXPECT_FLOAT_EQ(rp2.x(), 0.2f);
  EXPECT_FLOAT_EQ(rp2.y(), -0.1f);
}

TEST_F(EpipolarTest, TestEpipolarLineSegmentTranslateX) {
  auto w = epipolarDepthImagePrework(TRANSLATE_X, IDENTITY);
  auto E = essentialMatrixForCameras(TRANSLATE_X, IDENTITY);
  auto [lp1, lp2] = epipolarDepthPointPrework(w, LEFT_PT).lineSegment;
  auto [mp1, mp2] = epipolarDepthPointPrework(w, MID_PT).lineSegment;
  auto [rp1, rp2] = epipolarDepthPointPrework(w, RIGHT_PT).lineSegment;

  // Check that the ends of the segment and its midpoint are along the epipolar line.
  auto lp3 = 0.5f * (lp1 + lp2);
  auto mp3 = 0.5f * (mp1 + mp2);
  auto rp3 = 0.5f * (rp1 + rp2);
  EXPECT_FLOAT_EQ(distanceFromEpipolar(E, LEFT_PT, asPoint(lp1)), 0.0f);
  EXPECT_FLOAT_EQ(distanceFromEpipolar(E, MID_PT, asPoint(mp1)), 0.0f);
  EXPECT_FLOAT_EQ(distanceFromEpipolar(E, RIGHT_PT, asPoint(rp1)), 0.0f);
  EXPECT_FLOAT_EQ(distanceFromEpipolar(E, LEFT_PT, asPoint(lp2)), 0.0f);
  EXPECT_FLOAT_EQ(distanceFromEpipolar(E, MID_PT, asPoint(mp2)), 0.0f);
  EXPECT_FLOAT_EQ(distanceFromEpipolar(E, RIGHT_PT, asPoint(rp2)), 0.0f);
  EXPECT_FLOAT_EQ(distanceFromEpipolar(E, LEFT_PT, asPoint(lp3)), 0.0f);
  EXPECT_FLOAT_EQ(distanceFromEpipolar(E, MID_PT, asPoint(mp3)), 0.0f);
  EXPECT_FLOAT_EQ(distanceFromEpipolar(E, RIGHT_PT, asPoint(rp3)), 0.0f);

  // There's no rotation, so far points should be at their original spots.
  EXPECT_FLOAT_EQ(lp1.x(), -0.2f);
  EXPECT_FLOAT_EQ(lp1.y(), 0.1f);
  EXPECT_FLOAT_EQ(mp1.x(), 0.0f);
  EXPECT_FLOAT_EQ(mp1.y(), 0.0f);
  EXPECT_FLOAT_EQ(rp1.x(), 0.2f);
  EXPECT_FLOAT_EQ(rp1.y(), -0.1f);

  // Near points should have a fixed translation scaled by nearDist, but should otherwise
  // be offset by their original values.
  auto nearTrans = 0.05 / w.nearDist;
  EXPECT_FLOAT_EQ(lp2.x(), nearTrans - 0.2f);
  EXPECT_FLOAT_EQ(lp2.y(), 0.1f);
  EXPECT_FLOAT_EQ(mp2.x(), nearTrans);
  EXPECT_FLOAT_EQ(mp2.y(), 0.0f);
  EXPECT_FLOAT_EQ(rp2.x(), nearTrans + 0.2f);
  EXPECT_FLOAT_EQ(rp2.y(), -0.1f);
}

TEST_F(EpipolarTest, TestEpipolarLineSegmentRotateY) {
  auto w = epipolarDepthImagePrework(ROTATE_Y, IDENTITY);
  auto [lp1, lp2] = epipolarDepthPointPrework(w, LEFT_PT).lineSegment;
  auto [mp1, mp2] = epipolarDepthPointPrework(w, MID_PT).lineSegment;
  auto [rp1, rp2] = epipolarDepthPointPrework(w, RIGHT_PT).lineSegment;

  // Since this is a pure rotation, we expect the close and far points to be the same.
  auto elp = (ROTATE_Y * LEFT_PT.extrude()).flatten();
  auto emp = (ROTATE_Y * MID_PT.extrude()).flatten();
  auto erp = (ROTATE_Y * RIGHT_PT.extrude()).flatten();

  EXPECT_FLOAT_EQ(lp1.x(), elp.x());
  EXPECT_FLOAT_EQ(lp1.y(), elp.y());
  EXPECT_FLOAT_EQ(mp1.x(), emp.x());
  EXPECT_FLOAT_EQ(mp1.y(), emp.y());
  EXPECT_FLOAT_EQ(rp1.x(), erp.x());
  EXPECT_FLOAT_EQ(rp1.y(), erp.y());

  EXPECT_FLOAT_EQ(lp2.x(), elp.x());
  EXPECT_FLOAT_EQ(lp2.y(), elp.y());
  EXPECT_FLOAT_EQ(mp2.x(), emp.x());
  EXPECT_FLOAT_EQ(mp2.y(), emp.y());
  EXPECT_FLOAT_EQ(rp2.x(), erp.x());
  EXPECT_FLOAT_EQ(rp2.y(), erp.y());
}

TEST_F(EpipolarTest, TestEpipolarLineSegmentTranslateZ) {
  auto w = epipolarDepthImagePrework(TRANSLATE_Z, IDENTITY);
  auto E = essentialMatrixForCameras(TRANSLATE_Z, IDENTITY);
  auto [lp1, lp2] = epipolarDepthPointPrework(w, LEFT_PT).lineSegment;
  auto [mp1, mp2] = epipolarDepthPointPrework(w, MID_PT).lineSegment;
  auto [rp1, rp2] = epipolarDepthPointPrework(w, RIGHT_PT).lineSegment;

  // Check that the ends of the segment and its midpoint are along the epipolar line.
  // Skip checks for mid-point because it's on the focus of expansion.
  auto lp3 = 0.5f * (lp1 + lp2);
  auto rp3 = 0.5f * (rp1 + rp2);
  EXPECT_NEAR(distanceFromEpipolar(E, LEFT_PT, asPoint(lp1)), 0.0f, 1e-8f);
  EXPECT_NEAR(distanceFromEpipolar(E, RIGHT_PT, asPoint(rp1)), 0.0f, 1e-8f);
  EXPECT_NEAR(distanceFromEpipolar(E, LEFT_PT, asPoint(lp2)), 0.0f, 1e-8f);
  EXPECT_NEAR(distanceFromEpipolar(E, RIGHT_PT, asPoint(rp2)), 0.0f, 1e-8f);
  EXPECT_NEAR(distanceFromEpipolar(E, LEFT_PT, asPoint(lp3)), 0.0f, 1e-8f);
  EXPECT_NEAR(distanceFromEpipolar(E, RIGHT_PT, asPoint(rp3)), 0.0f, 1e-8f);

  // Far points should be unchanged since there is no rotation.
  EXPECT_FLOAT_EQ(lp1.x(), -0.2f);
  EXPECT_FLOAT_EQ(lp1.y(), 0.1f);
  EXPECT_FLOAT_EQ(mp1.x(), 0.0f);
  EXPECT_FLOAT_EQ(mp1.y(), 0.0f);
  EXPECT_FLOAT_EQ(rp1.x(), 0.2f);
  EXPECT_FLOAT_EQ(rp1.y(), -0.1f);

  // Near mid-points should be unchanged since they are on the focus of expansion.
  EXPECT_FLOAT_EQ(mp2.x(), 0.0f);
  EXPECT_FLOAT_EQ(mp2.y(), 0.0f);

  // The near points should be along the rays at distance 0.05 + nearDist from the first
  // camera, projected into the second camera and flattened.
  float s = 0.05f + w.nearDist;
  auto elp = (TRANSLATE_Z * HPoint3{LEFT_PT.x() * s, LEFT_PT.y() * s, s}).flatten();
  auto erp = (TRANSLATE_Z * HPoint3{RIGHT_PT.x() * s, RIGHT_PT.y() * s, s}).flatten();

  EXPECT_FLOAT_EQ(lp2.x(), elp.x());
  EXPECT_FLOAT_EQ(lp2.y(), elp.y());
  EXPECT_FLOAT_EQ(rp2.x(), erp.x());
  EXPECT_FLOAT_EQ(rp2.y(), erp.y());
}

TEST_F(EpipolarTest, TestEpipolarLineSegmentTranslateZReverse) {
  auto w = epipolarDepthImagePrework(TRANSLATE_Z.inv(), IDENTITY);
  auto E = essentialMatrixForCameras(TRANSLATE_Z.inv(), IDENTITY);
  auto [lp1, lp2] = epipolarDepthPointPrework(w, LEFT_PT).lineSegment;
  auto [mp1, mp2] = epipolarDepthPointPrework(w, MID_PT).lineSegment;
  auto [rp1, rp2] = epipolarDepthPointPrework(w, RIGHT_PT).lineSegment;

  // Check that the ends of the segment and its midpoint are along the epipolar line.
  // Skip checks for mid-point because it's on the focus of expansion.
  auto lp3 = 0.5f * (lp1 + lp2);
  auto rp3 = 0.5f * (rp1 + rp2);
  EXPECT_NEAR(distanceFromEpipolar(E, LEFT_PT, asPoint(lp1)), 0.0f, 1e-8f);
  EXPECT_NEAR(distanceFromEpipolar(E, RIGHT_PT, asPoint(rp1)), 0.0f, 1e-8f);
  EXPECT_NEAR(distanceFromEpipolar(E, LEFT_PT, asPoint(lp2)), 0.0f, 1e-8f);
  EXPECT_NEAR(distanceFromEpipolar(E, RIGHT_PT, asPoint(rp2)), 0.0f, 1e-8f);
  EXPECT_NEAR(distanceFromEpipolar(E, LEFT_PT, asPoint(lp3)), 0.0f, 1e-8f);
  EXPECT_NEAR(distanceFromEpipolar(E, RIGHT_PT, asPoint(rp3)), 0.0f, 1e-8f);

  // Far points should be unchanged since there is no rotation.
  EXPECT_FLOAT_EQ(lp1.x(), -0.2f);
  EXPECT_FLOAT_EQ(lp1.y(), 0.1f);
  EXPECT_FLOAT_EQ(mp1.x(), 0.0f);
  EXPECT_FLOAT_EQ(mp1.y(), 0.0f);
  EXPECT_FLOAT_EQ(rp1.x(), 0.2f);
  EXPECT_FLOAT_EQ(rp1.y(), -0.1f);

  // Near mid-points should be unchanged since they are on the focus of expansion.
  EXPECT_FLOAT_EQ(mp2.x(), 0.0f);
  EXPECT_FLOAT_EQ(mp2.y(), 0.0f);

  // The near points should be along the rays at distance nearDist from the first
  // camera, projected into the second camera and flattened.
  float s = w.nearDist;
  auto elp = (TRANSLATE_Z.inv() * HPoint3{LEFT_PT.x() * s, LEFT_PT.y() * s, s}).flatten();
  auto erp = (TRANSLATE_Z.inv() * HPoint3{RIGHT_PT.x() * s, RIGHT_PT.y() * s, s}).flatten();

  EXPECT_FLOAT_EQ(lp2.x(), elp.x());
  EXPECT_FLOAT_EQ(lp2.y(), elp.y());
  EXPECT_FLOAT_EQ(rp2.x(), erp.x());
  EXPECT_FLOAT_EQ(rp2.y(), erp.y());
}

TEST_F(EpipolarTest, ClosestLineSegmentPointHorizontalLine) {
  auto w = closestLineSegmentPointPrework({{-1.0f, 5.0f}, {3.0f, 5.0f}});
  // Point on the line.
  {
    auto p = closestLineSegmentPoint(w, {0.0f, 5.0f});
    EXPECT_FLOAT_EQ(p.x(), 0.0f);
    EXPECT_FLOAT_EQ(p.y(), 5.0f);
  }
  // Point above the line.
  {
    auto p = closestLineSegmentPoint(w, {2.0f, 7.0f});
    EXPECT_FLOAT_EQ(p.x(), 2.0f);
    EXPECT_FLOAT_EQ(p.y(), 5.0f);
  }
  // Point below the line.
  {
    auto p = closestLineSegmentPoint(w, {-0.5f, 2.0f});
    EXPECT_FLOAT_EQ(p.x(), -0.5f);
    EXPECT_FLOAT_EQ(p.y(), 5.0f);
  }
  // Point left of the line.
  {
    auto p = closestLineSegmentPoint(w, {-1.5f, 1.0f});
    EXPECT_FLOAT_EQ(p.x(), -1.0f);
    EXPECT_FLOAT_EQ(p.y(), 5.0f);
  }
  // Point right of the line.
  {
    auto p = closestLineSegmentPoint(w, {4.5f, 6.0f});
    EXPECT_FLOAT_EQ(p.x(), 3.0f);
    EXPECT_FLOAT_EQ(p.y(), 5.0f);
  }
}

TEST_F(EpipolarTest, ClosestLineSegmentPointVerticalLine) {
  auto w = closestLineSegmentPointPrework({{5.0f, -1.0f}, {5.0f, 3.0f}});
  // Point on the line.
  {
    auto p = closestLineSegmentPoint(w, {5.0f, 0.0f});
    EXPECT_FLOAT_EQ(p.x(), 5.0f);
    EXPECT_FLOAT_EQ(p.y(), 0.0f);
  }
  // Point right of the line.
  {
    auto p = closestLineSegmentPoint(w, {7.0f, 2.0f});
    EXPECT_FLOAT_EQ(p.x(), 5.0f);
    EXPECT_FLOAT_EQ(p.y(), 2.0f);
  }
  // Point left of the line.
  {
    auto p = closestLineSegmentPoint(w, {2.0f, -0.5f});
    EXPECT_FLOAT_EQ(p.x(), 5.0f);
    EXPECT_FLOAT_EQ(p.y(), -0.5f);
  }
  // Point below the line.
  {
    auto p = closestLineSegmentPoint(w, {1.0f, -1.5f});
    EXPECT_FLOAT_EQ(p.x(), 5.0f);
    EXPECT_FLOAT_EQ(p.y(), -1.0f);
  }
  // Point above the line.
  {
    auto p = closestLineSegmentPoint(w, {6.0f, 4.5f});
    EXPECT_FLOAT_EQ(p.x(), 5.0f);
    EXPECT_FLOAT_EQ(p.y(), 3.0f);
  }
}

TEST_F(EpipolarTest, ClosestLineSegmentPointDiagonalLine) {
  auto w = closestLineSegmentPointPrework({{-1.0f, -1.0f}, {1.0f, 1.0f}});
  // Point on the line.
  {
    auto p = closestLineSegmentPoint(w, {0.5f, 0.5f});
    EXPECT_FLOAT_EQ(p.x(), 0.5f);
    EXPECT_FLOAT_EQ(p.y(), 0.5f);
  }
  // Point right of the line.
  {
    auto p = closestLineSegmentPoint(w, {-.25f + .3f, -.25f - .3f});
    EXPECT_FLOAT_EQ(p.x(), -.25f);
    EXPECT_FLOAT_EQ(p.y(), -.25f);
  }
  // Point left of the line.
  {
    auto p = closestLineSegmentPoint(w, {0.3f - .25f, 0.3f + .25f});
    EXPECT_FLOAT_EQ(p.x(), 0.3f);
    EXPECT_FLOAT_EQ(p.y(), 0.3f);
  }
  // Point below the line.
  {
    auto p = closestLineSegmentPoint(w, {-1.5f, -2.0f});
    EXPECT_FLOAT_EQ(p.x(), -1.0f);
    EXPECT_FLOAT_EQ(p.y(), -1.0f);
  }
  // Point above the line.
  {
    auto p = closestLineSegmentPoint(w, {2.0f, 1.5f});
    EXPECT_FLOAT_EQ(p.x(), 1.0f);
    EXPECT_FLOAT_EQ(p.y(), 1.0f);
  }
}

TEST_F(EpipolarTest, ClosestLineSegmentPointPoint) {
  auto w = closestLineSegmentPointPrework({{3.0f, -2.0f}, {3.0f, -2.0f}});
  // Point on the line.
  {
    auto p = closestLineSegmentPoint(w, {3.0f, -2.0f});
    EXPECT_FLOAT_EQ(p.x(), 3.0f);
    EXPECT_FLOAT_EQ(p.y(), -2.0f);
  }
  // Some other points.
  {
    auto p = closestLineSegmentPoint(w, {-.25f + .3f, -.25f - .3f});
    EXPECT_FLOAT_EQ(p.x(), 3.0f);
    EXPECT_FLOAT_EQ(p.y(), -2.0f);
  }
  {
    auto p = closestLineSegmentPoint(w, {0.3f - .25f, 0.3f + .25f});
    EXPECT_FLOAT_EQ(p.x(), 3.0f);
    EXPECT_FLOAT_EQ(p.y(), -2.0f);
  }
  {
    auto p = closestLineSegmentPoint(w, {-1.5f, -2.0f});
    EXPECT_FLOAT_EQ(p.x(), 3.0f);
    EXPECT_FLOAT_EQ(p.y(), -2.0f);
  }
  {
    auto p = closestLineSegmentPoint(w, {2.0f, 1.5f});
    EXPECT_FLOAT_EQ(p.x(), 3.0f);
    EXPECT_FLOAT_EQ(p.y(), -2.0f);
  }
}

TEST_F(EpipolarTest, TestDepthOfEpipolarPointMoveRight) {
  auto cam = HMatrixGen::translateX(0.1f);
  auto iw = epipolarDepthImagePrework(HMatrixGen::i(), cam);
  auto centerPt2 = asVector((cam.inv() * CENTER_PT_3).flatten());
  auto xPt2 = asVector((cam.inv() * X_PT_3).flatten());
  auto yPt2 = asVector((cam.inv() * Y_PT_3).flatten());
  auto diagPt2 = asVector((cam.inv() * DIAG_PT_3).flatten());

  auto centerDepth = depth(iw, CENTER_PT, centerPt2);
  auto xDepth = depth(iw, X_PT, xPt2);
  auto yDepth = depth(iw, Y_PT, yPt2);
  auto diagDepth = depth(iw, DIAG_PT, diagPt2);

  EXPECT_EQ(centerDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(xDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(yDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(diagDepth.status, EpipolarDepthResult::Status::OK);

  EXPECT_FLOAT_EQ(centerDepth.depthInWords, CENTER_PT_3.z());
  EXPECT_FLOAT_EQ(xDepth.depthInWords, X_PT_3.z());
  EXPECT_FLOAT_EQ(yDepth.depthInWords, Y_PT_3.z());
  EXPECT_FLOAT_EQ(diagDepth.depthInWords, DIAG_PT_3.z());

  // We expect x/y to have the same uncertainty because they're at the same depth and there's no z
  // translation. After that, we expect farther points to have more uncertainty.
  EXPECT_FLOAT_EQ(xDepth.certainty, yDepth.certainty);
  EXPECT_LT(diagDepth.certainty, yDepth.certainty);
  EXPECT_LT(centerDepth.certainty, diagDepth.certainty);
}

TEST_F(EpipolarTest, TestDepthOfEpipolarPointMoveRightDifferentScales) {
  auto cam = HMatrixGen::translateX(0.1f);
  auto iw = epipolarDepthImagePrework(HMatrixGen::i(), cam);
  auto xPt2 = asVector((cam.inv() * X_PT_3).flatten());
  auto yPt2 = asVector((cam.inv() * Y_PT_3).flatten());

  auto xDepth = depth(iw, X_PT, xPt2, 1);
  auto yDepth = depth(iw, Y_PT, yPt2, 3);

  EXPECT_EQ(xDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(yDepth.status, EpipolarDepthResult::Status::OK);

  // Points are triangulated at the same depth
  EXPECT_FLOAT_EQ(xDepth.depthInWords, X_PT_3.z());
  EXPECT_FLOAT_EQ(yDepth.depthInWords, Y_PT_3.z());

  // Since y point is at a scale with a larger patch size, it has less location certainty.
  EXPECT_GT(xDepth.certainty, yDepth.certainty);
}

TEST_F(EpipolarTest, TestDepthOfEpipolarPointMoveLeft) {
  auto cam = HMatrixGen::translateX(-0.2f);
  auto iw = epipolarDepthImagePrework(HMatrixGen::i(), cam);
  auto centerPt2 = asVector((cam.inv() * CENTER_PT_3).flatten());
  auto xPt2 = asVector((cam.inv() * X_PT_3).flatten());
  auto yPt2 = asVector((cam.inv() * Y_PT_3).flatten());
  auto diagPt2 = asVector((cam.inv() * DIAG_PT_3).flatten());

  auto centerDepth = depth(iw, CENTER_PT, centerPt2);
  auto xDepth = depth(iw, X_PT, xPt2);
  auto yDepth = depth(iw, Y_PT, yPt2);
  auto diagDepth = depth(iw, DIAG_PT, diagPt2);

  EXPECT_EQ(centerDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(xDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(yDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(diagDepth.status, EpipolarDepthResult::Status::OK);

  EXPECT_FLOAT_EQ(centerDepth.depthInWords, CENTER_PT_3.z());
  EXPECT_FLOAT_EQ(xDepth.depthInWords, X_PT_3.z());
  EXPECT_FLOAT_EQ(yDepth.depthInWords, Y_PT_3.z());
  EXPECT_FLOAT_EQ(diagDepth.depthInWords, DIAG_PT_3.z());

  // We expect x/y to have the same uncertainty because they're at the same depth and there's no z
  // translation. After that, we expect farther points to have more uncertainty.
  EXPECT_FLOAT_EQ(xDepth.certainty, yDepth.certainty);
  EXPECT_LT(diagDepth.certainty, yDepth.certainty);
  EXPECT_LT(centerDepth.certainty, diagDepth.certainty);
}

TEST_F(EpipolarTest, TestDepthOfEpipolarPointMoveUp) {
  auto cam = HMatrixGen::translateY(0.1f);
  auto iw = epipolarDepthImagePrework(HMatrixGen::i(), cam);
  auto centerPt2 = asVector((cam.inv() * CENTER_PT_3).flatten());
  auto xPt2 = asVector((cam.inv() * X_PT_3).flatten());
  auto yPt2 = asVector((cam.inv() * Y_PT_3).flatten());
  auto diagPt2 = asVector((cam.inv() * DIAG_PT_3).flatten());

  auto centerDepth = depth(iw, CENTER_PT, centerPt2);
  auto xDepth = depth(iw, X_PT, xPt2);
  auto yDepth = depth(iw, Y_PT, yPt2);
  auto diagDepth = depth(iw, DIAG_PT, diagPt2);

  EXPECT_EQ(centerDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(xDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(yDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(diagDepth.status, EpipolarDepthResult::Status::OK);

  EXPECT_FLOAT_EQ(centerDepth.depthInWords, CENTER_PT_3.z());
  EXPECT_FLOAT_EQ(xDepth.depthInWords, X_PT_3.z());
  EXPECT_FLOAT_EQ(yDepth.depthInWords, Y_PT_3.z());
  EXPECT_FLOAT_EQ(diagDepth.depthInWords, DIAG_PT_3.z());

  // We expect x/y to have the same uncertainty because they're at the same depth and there's no z
  // translation. After that, we expect farther points to have more uncertainty.
  EXPECT_FLOAT_EQ(xDepth.certainty, yDepth.certainty);
  EXPECT_LT(diagDepth.certainty, yDepth.certainty);
  EXPECT_LT(centerDepth.certainty, diagDepth.certainty);
}

TEST_F(EpipolarTest, TestDepthOfEpipolarPointMoveDown) {
  auto cam = HMatrixGen::translateY(-0.2f);
  auto iw = epipolarDepthImagePrework(HMatrixGen::i(), cam);
  auto centerPt2 = asVector((cam.inv() * CENTER_PT_3).flatten());
  auto xPt2 = asVector((cam.inv() * X_PT_3).flatten());
  auto yPt2 = asVector((cam.inv() * Y_PT_3).flatten());
  auto diagPt2 = asVector((cam.inv() * DIAG_PT_3).flatten());

  auto centerDepth = depth(iw, CENTER_PT, centerPt2);
  auto xDepth = depth(iw, X_PT, xPt2);
  auto yDepth = depth(iw, Y_PT, yPt2);
  auto diagDepth = depth(iw, DIAG_PT, diagPt2);

  EXPECT_EQ(centerDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(xDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(yDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(diagDepth.status, EpipolarDepthResult::Status::OK);

  EXPECT_FLOAT_EQ(centerDepth.depthInWords, CENTER_PT_3.z());
  EXPECT_FLOAT_EQ(xDepth.depthInWords, X_PT_3.z());
  EXPECT_FLOAT_EQ(yDepth.depthInWords, Y_PT_3.z());
  EXPECT_FLOAT_EQ(diagDepth.depthInWords, DIAG_PT_3.z());

  // We expect x/y to have the same uncertainty because they're at the same depth and there's no z
  // translation. After that, we expect farther points to have more uncertainty.
  EXPECT_FLOAT_EQ(xDepth.certainty, yDepth.certainty);
  EXPECT_LT(diagDepth.certainty, yDepth.certainty);
  EXPECT_LT(centerDepth.certainty, diagDepth.certainty);
}

TEST_F(EpipolarTest, TestDepthOfEpipolarPointMoveIn) {
  auto cam = HMatrixGen::translateZ(0.1f);
  auto iw = epipolarDepthImagePrework(HMatrixGen::i(), cam);
  auto centerPt2 = asVector((cam.inv() * CENTER_PT_3).flatten());
  auto xPt2 = asVector((cam.inv() * X_PT_3).flatten());
  auto yPt2 = asVector((cam.inv() * Y_PT_3).flatten());
  auto diagPt2 = asVector((cam.inv() * DIAG_PT_3).flatten());

  auto centerDepth = depth(iw, CENTER_PT, centerPt2);
  auto xDepth = depth(iw, X_PT, xPt2);
  auto yDepth = depth(iw, Y_PT, yPt2);
  auto diagDepth = depth(iw, DIAG_PT, diagPt2);

  EXPECT_EQ(centerDepth.status, EpipolarDepthResult::Status::FOCUS_OF_EXPANSION);
  EXPECT_EQ(xDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(yDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(diagDepth.status, EpipolarDepthResult::Status::OK);

  EXPECT_NEAR(xDepth.depthInWords, X_PT_3.z(), 5e-7f);
  EXPECT_NEAR(yDepth.depthInWords, Y_PT_3.z(), 5e-7f);
  EXPECT_NEAR(diagDepth.depthInWords, DIAG_PT_3.z(), 5e-7f);

  // We expect x to have the more certainty than y because it's farther from the focus of
  // expansion. After that, we expect farther points to have more uncertainty.
  EXPECT_GT(xDepth.certainty, yDepth.certainty);
  EXPECT_LT(diagDepth.certainty, yDepth.certainty);
  EXPECT_LT(centerDepth.certainty, diagDepth.certainty);
}

TEST_F(EpipolarTest, TestDepthOfEpipolarPointMoveOut) {
  auto cam = HMatrixGen::translateZ(-0.2f);
  auto iw = epipolarDepthImagePrework(HMatrixGen::i(), cam);
  auto centerPt2 = asVector((cam.inv() * CENTER_PT_3).flatten());
  auto xPt2 = asVector((cam.inv() * X_PT_3).flatten());
  auto yPt2 = asVector((cam.inv() * Y_PT_3).flatten());
  auto diagPt2 = asVector((cam.inv() * DIAG_PT_3).flatten());

  auto centerDepth = depth(iw, CENTER_PT, centerPt2);
  auto xDepth = depth(iw, X_PT, xPt2);
  auto yDepth = depth(iw, Y_PT, yPt2);
  auto diagDepth = depth(iw, DIAG_PT, diagPt2);

  EXPECT_EQ(centerDepth.status, EpipolarDepthResult::Status::FOCUS_OF_EXPANSION);
  EXPECT_EQ(xDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(yDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(diagDepth.status, EpipolarDepthResult::Status::OK);

  EXPECT_NEAR(xDepth.depthInWords, X_PT_3.z(), 5e-7f);
  EXPECT_NEAR(yDepth.depthInWords, Y_PT_3.z(), 5e-7f);
  EXPECT_NEAR(diagDepth.depthInWords, DIAG_PT_3.z(), 5e-7f);

  // We expect x to have the more certainty than y because it's farther from the focus of
  // expansion. After that, we expect farther points to have more uncertainty.
  EXPECT_GT(xDepth.certainty, yDepth.certainty);
  EXPECT_LT(diagDepth.certainty, yDepth.certainty);
  EXPECT_LT(centerDepth.certainty, diagDepth.certainty);
}

TEST_F(EpipolarTest, TestDepthOfEpipolarPointRotation) {
  auto cam = HMatrixGen::rotationD(0.0f, 30.0f, 0.0f);

  auto iw = epipolarDepthImagePrework(HMatrixGen::i(), cam);
  auto centerPt2 = asVector((cam.inv() * CENTER_PT_3).flatten());
  auto xPt2 = asVector((cam.inv() * X_PT_3).flatten());
  auto yPt2 = asVector((cam.inv() * Y_PT_3).flatten());
  auto diagPt2 = asVector((cam.inv() * DIAG_PT_3).flatten());

  auto centerDepth = depth(iw, CENTER_PT, centerPt2);
  auto xDepth = depth(iw, X_PT, xPt2);
  auto yDepth = depth(iw, Y_PT, yPt2);
  auto diagDepth = depth(iw, DIAG_PT, diagPt2);

  EXPECT_EQ(centerDepth.status, EpipolarDepthResult::Status::PURE_ROTATION);
  EXPECT_EQ(xDepth.status, EpipolarDepthResult::Status::PURE_ROTATION);
  EXPECT_EQ(yDepth.status, EpipolarDepthResult::Status::PURE_ROTATION);
  EXPECT_EQ(diagDepth.status, EpipolarDepthResult::Status::PURE_ROTATION);
}

TEST_F(EpipolarTest, TestDepthOfEpipolarPointFullMotion) {
  auto iw = epipolarDepthImagePrework(HMatrixGen::i(), FULL);
  auto centerPt2 = asVector((FULL.inv() * CENTER_PT_3).flatten());
  auto xPt2 = asVector((FULL.inv() * X_PT_3).flatten());
  auto yPt2 = asVector((FULL.inv() * Y_PT_3).flatten());
  auto diagPt2 = asVector((FULL.inv() * DIAG_PT_3).flatten());

  auto centerDepth = depth(iw, CENTER_PT, centerPt2);
  auto xDepth = depth(iw, X_PT, xPt2);
  auto yDepth = depth(iw, Y_PT, yPt2);
  auto diagDepth = depth(iw, DIAG_PT, diagPt2);

  EXPECT_EQ(centerDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(xDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(yDepth.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(diagDepth.status, EpipolarDepthResult::Status::OK);

  EXPECT_NEAR(centerDepth.depthInWords, CENTER_PT_3.z(), 4e-6f);
  EXPECT_NEAR(xDepth.depthInWords, X_PT_3.z(), 8e-7f);
  EXPECT_NEAR(yDepth.depthInWords, Y_PT_3.z(), 8e-7f);
  EXPECT_NEAR(diagDepth.depthInWords, DIAG_PT_3.z(), 3e-6f);
}

TEST_F(EpipolarTest, TestDepthOfEpipolarPointTranslationBaseline) {
  // Two cameras in the same direction but one has a larger movement.
  auto cam1 = HMatrixGen::translateX(0.05f);
  auto cam2 = HMatrixGen::translateX(0.1f);
  auto iw1 = epipolarDepthImagePrework(HMatrixGen::i(), cam1);
  auto iw2 = epipolarDepthImagePrework(HMatrixGen::i(), cam2);
  auto xPt1 = asVector((cam1.inv() * X_PT_3).flatten());
  auto xPt2 = asVector((cam2.inv() * X_PT_3).flatten());

  auto xDepth1 = depth(iw1, X_PT, xPt1);
  auto xDepth2 = depth(iw2, X_PT, xPt2);

  EXPECT_EQ(xDepth1.status, EpipolarDepthResult::Status::OK);
  EXPECT_EQ(xDepth2.status, EpipolarDepthResult::Status::OK);

  // Both have the same depth.
  EXPECT_FLOAT_EQ(xDepth1.depthInWords, X_PT_3.z());
  EXPECT_FLOAT_EQ(xDepth2.depthInWords, X_PT_3.z());

  // The camera with the higher baseline should have more certainty.
  EXPECT_GT(xDepth2.certainty, xDepth1.certainty);
}

TEST_F(EpipolarTest, TestCombineDepths) {
  auto depth = combineDepths(
    combineDepths(
      combineDepths(
        {1.0f, 0.0f, EpipolarDepthResult::Status::FOCUS_OF_EXPANSION},
        {1.0f, 2.0f, EpipolarDepthResult::Status::OK}),
      {1.0f, 0.0f, EpipolarDepthResult::Status::PURE_ROTATION}),
    {2.0f, 1.0f, EpipolarDepthResult::Status::OK});

  EXPECT_FLOAT_EQ(depth.depthInWords, 4.0f / 3.0f);
  EXPECT_FLOAT_EQ(depth.certainty, 3.0f);
  EXPECT_EQ(depth.status, EpipolarDepthResult::Status::OK);
}

TEST_F(EpipolarTest, TestCombineDepthsVector) {
  auto depth = combineDepths({
    {1.0f, 0.0f, EpipolarDepthResult::Status::FOCUS_OF_EXPANSION},
    {1.0f, 2.0f, EpipolarDepthResult::Status::OK},
    {1.0f, 0.0f, EpipolarDepthResult::Status::PURE_ROTATION},
    {2.0f, 1.0f, EpipolarDepthResult::Status::OK},
  });

  EXPECT_FLOAT_EQ(depth.depthInWords, 4.0f / 3.0f);
  EXPECT_FLOAT_EQ(depth.certainty, 3.0);
  EXPECT_EQ(depth.status, EpipolarDepthResult::Status::OK);
}

TEST_F(EpipolarTest, TestCombineNoGoodDepths) {
  auto depth = combineDepths(
    {1.0f, 0.0f, EpipolarDepthResult::Status::FOCUS_OF_EXPANSION},
    {1.0f, 0.0f, EpipolarDepthResult::Status::PURE_ROTATION});

  // The camera with the higher baseline should have more certainty.
  EXPECT_EQ(depth.status, EpipolarDepthResult::Status::TOO_UNCERTAIN);
}

TEST_F(EpipolarTest, TestCombineNoGoodDepthsVector) {
  auto depth = combineDepths({
    {1.0f, 0.0f, EpipolarDepthResult::Status::FOCUS_OF_EXPANSION},
    {1.0f, 0.0f, EpipolarDepthResult::Status::PURE_ROTATION},
  });

  // The camera with the higher baseline should have more certainty.
  EXPECT_EQ(depth.status, EpipolarDepthResult::Status::TOO_UNCERTAIN);
}

TEST_F(EpipolarTest, TestDecombineDepths) {
  EpipolarDepthResult depth{4.0f / 3.0f, 3.0f, EpipolarDepthResult::Status::OK};
  depth = decombineDepths(depth, {2.0f, 1.0f, EpipolarDepthResult::Status::OK});

  EXPECT_FLOAT_EQ(depth.depthInWords, 1.0f);
  EXPECT_FLOAT_EQ(depth.certainty, 2.0f);
  EXPECT_EQ(depth.status, EpipolarDepthResult::Status::OK);

  depth = decombineDepths(depth, {1.0f, 0.0f, EpipolarDepthResult::Status::PURE_ROTATION});

  EXPECT_FLOAT_EQ(depth.depthInWords, 1.0f);
  EXPECT_FLOAT_EQ(depth.certainty, 2.0f);
  EXPECT_EQ(depth.status, EpipolarDepthResult::Status::OK);

  depth = decombineDepths(depth, {1.0f, 2.0f, EpipolarDepthResult::Status::OK});

  EXPECT_FLOAT_EQ(depth.depthInWords, 1.0f);
  EXPECT_FLOAT_EQ(depth.certainty, 0.0f);
  EXPECT_EQ(depth.status, EpipolarDepthResult::Status::TOO_UNCERTAIN);

  depth = decombineDepths(depth, {1.0f, 0.0f, EpipolarDepthResult::Status::FOCUS_OF_EXPANSION});

  EXPECT_FLOAT_EQ(depth.depthInWords, 1.0f);
  EXPECT_FLOAT_EQ(depth.certainty, 0.0f);
  EXPECT_EQ(depth.status, EpipolarDepthResult::Status::TOO_UNCERTAIN);
}

TEST_F(EpipolarTest, TestInexactPointDepth) {
  HMatrix wordsCam = HMatrixGen::i();
  HMatrix dictCam = cameraMotion(
    HVector3{-0.0312544f, 0.0664566f, 0.0509237f},
    {0.99934f, 0.00501435f, 0.0132628f, -0.0334521f});
  auto wordsPt = HPoint2{0.405741f, 0.071959f};
  auto dictPt = HVector2{0.386807f, 0.154542f};
  auto iw = epipolarDepthImagePrework(wordsCam, dictCam);
  auto d = depth(iw, wordsPt, dictPt);
  EXPECT_EQ(d.status, EpipolarDepthResult::Status::FOCUS_OF_EXPANSION);
}

}  // namespace c8
