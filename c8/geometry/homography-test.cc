// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//c8/geometry:egomotion",
    "//c8/geometry:homography",
    "//c8/geometry:worlds",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x9e817a02);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/homography.h"
#include "c8/geometry/worlds.h"
#include "c8/quaternion.h"
#include "c8/stats/scope-timer.h"

using testing::Eq;
using testing::Pointwise;

namespace c8 {

testing::AssertionResult AreEqual(Quaternion q1, Quaternion q2, float epsilon = 1e-6) {
  // The straightforward case
  if (
    (fabs(q1.w() - q2.w()) < epsilon) && (fabs(q1.x() - q2.x()) < epsilon)
    && (fabs(q1.y() - q2.y()) < epsilon) && (fabs(q1.z() - q2.z()) < epsilon)) {
    return testing::AssertionSuccess();
  }
  // The negated case
  if (
    (fabs(q1.w() - -q2.w()) < epsilon) && (fabs(q1.x() - -q2.x()) < epsilon)
    && (fabs(q1.y() - -q2.y()) < epsilon) && (fabs(q1.z() - -q2.z()) < epsilon)) {
    return testing::AssertionSuccess();
  }

  return testing::AssertionFailure() << "q1 " << q1.toString().c_str() << " does not equal "
                                     << q2.toString().c_str() << " with epsilon " << epsilon;
}

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }
MATCHER(AreEqual, "") { return testing::get<0>(arg) == testing::get<1>(arg); }

decltype(auto) equalsVector(const HVector3 &v) { return Pointwise(AreWithin(0.0001), v.data()); }
decltype(auto) equalsPoint(const HPoint2 &p) { return Pointwise(AreWithin(0.0001), p.data()); }
decltype(auto) equalsPoint(const HPoint3 &p) { return Pointwise(AreWithin(0.0001), p.data()); }
decltype(auto) equalsPoint(const HPoint3 &p, float threshold) {
  return Pointwise(AreWithin(threshold), p.data());
}

decltype(auto) equalsMatrix(const HMatrix &matrix) {
  return Pointwise(AreWithin(0.0001), matrix.data());
}

decltype(auto) equalsMask(const Vector<uint8_t> &mask) { return Pointwise(AreEqual(), mask); }
decltype(auto) equalsHorizon(const Vector<HorizonLocation> &l) { return Pointwise(AreEqual(), l); }

class HomographyTest : public ::testing::Test {};
/*
// Gets the 3d vector cross product of three points.
HVector3 cross3d(HPoint3 x1, HPoint3 x2, HPoint3 x3);

// Gets a unit vector perpendicular to the plane formed by three points, with the unit normal facing
// in the direction of the origin.
HVector3 getPlaneNormal(HPoint3 x1, HPoint3 x2, HPoint3 x3);
*/
TEST_F(HomographyTest, TestAreSame) {
  HPoint3 x1(1.0f, 2.0f, 3.0f);
  HPoint3 x2(1.0f, 2.0f, 3.0f);
  HPoint3 x3(1.0f, -2.0f, 3.0f);
  EXPECT_TRUE(areSame(x1, x2));
  EXPECT_FALSE(areSame(x1, x3));
}

TEST_F(HomographyTest, TestAreCollinear) {
  HPoint3 x1(1.0f, 2.0f, 3.0f);
  HPoint3 x2(1.0f, 2.0f, 3.0f);
  HPoint3 x3(1.0f, -2.0f, 3.0f);
  HPoint3 x4(1.0f, 0.0f, 3.0f);
  HPoint3 x5(2.0f, 2.0f, 3.0f);
  EXPECT_TRUE(areCollinear(x1, x2, x3));
  EXPECT_TRUE(areCollinear(x1, x2, x4));
  EXPECT_TRUE(areCollinear(x1, x2, x5));
  EXPECT_TRUE(areCollinear(x1, x3, x4));
  EXPECT_FALSE(areCollinear(x1, x3, x5));
  EXPECT_FALSE(areCollinear(x1, x4, x5));
}

TEST_F(HomographyTest, TestCross3d) {
  HPoint3 x1(1.0f, 0.0f, 0.0f);
  HPoint3 x2(1.0f, 0.0f, 1.0f);
  HPoint3 x3(2.0f, 0.0f, 0.0f);

  // We use a left-handed coordinate system.
  EXPECT_THAT(cross3d(x1, x2, x3).data(), equalsVector(HVector3(0.0f, 1.0f, 0.0f)));
}

TEST_F(HomographyTest, TestGetPlaneNormal) {
  {
    HPoint3 x0(-1.0f, -1.0f, -1.0f);

    HPoint3 xp1(-1.0f, 0.0f, -1.0f);
    HPoint3 xp2(-1.0f, -1.0f, 0.0f);

    HPoint3 yp1(1.0f, -1.0f, -1.0f);
    HPoint3 yp2(-1.0f, -1.0f, 1.0f);

    HPoint3 zp1(-1.0f, -2.0f, -1.0f);
    HPoint3 zp2(-4.0f, -1.0f, -1.0f);

    EXPECT_THAT(getPlaneNormal(x0, xp1, xp2).data(), equalsVector(HVector3(1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(x0, xp2, xp1).data(), equalsVector(HVector3(1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(xp1, x0, xp2).data(), equalsVector(HVector3(1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(xp1, xp2, x0).data(), equalsVector(HVector3(1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(xp2, x0, xp1).data(), equalsVector(HVector3(1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(xp2, xp1, x0).data(), equalsVector(HVector3(1.0f, 0.0f, 0.0f)));

    EXPECT_THAT(getPlaneNormal(x0, yp1, yp2).data(), equalsVector(HVector3(0.0f, 1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(x0, yp2, yp1).data(), equalsVector(HVector3(0.0f, 1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(yp1, x0, yp2).data(), equalsVector(HVector3(0.0f, 1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(yp1, yp2, x0).data(), equalsVector(HVector3(0.0f, 1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(yp2, x0, yp1).data(), equalsVector(HVector3(0.0f, 1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(yp2, yp1, x0).data(), equalsVector(HVector3(0.0f, 1.0f, 0.0f)));

    EXPECT_THAT(getPlaneNormal(x0, zp1, zp2).data(), equalsVector(HVector3(0.0f, 0.0f, 1.0f)));
    EXPECT_THAT(getPlaneNormal(x0, zp2, zp1).data(), equalsVector(HVector3(0.0f, 0.0f, 1.0f)));
    EXPECT_THAT(getPlaneNormal(zp1, x0, zp2).data(), equalsVector(HVector3(0.0f, 0.0f, 1.0f)));
    EXPECT_THAT(getPlaneNormal(zp1, zp2, x0).data(), equalsVector(HVector3(0.0f, 0.0f, 1.0f)));
    EXPECT_THAT(getPlaneNormal(zp2, x0, zp1).data(), equalsVector(HVector3(0.0f, 0.0f, 1.0f)));
    EXPECT_THAT(getPlaneNormal(zp2, zp1, x0).data(), equalsVector(HVector3(0.0f, 0.0f, 1.0f)));
  }

  {
    HPoint3 x0(1.0f, 1.0f, 1.0f);

    HPoint3 xp1(1.0f, 0.0f, 1.0f);
    HPoint3 xp2(1.0f, 1.0f, 0.0f);

    HPoint3 yp1(-1.0f, 1.0f, 1.0f);
    HPoint3 yp2(1.0f, 1.0f, -1.0f);

    HPoint3 zp1(1.0f, 2.0f, 1.0f);
    HPoint3 zp2(4.0f, 1.0f, 1.0f);

    EXPECT_THAT(getPlaneNormal(x0, xp1, xp2).data(), equalsVector(HVector3(-1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(x0, xp2, xp1).data(), equalsVector(HVector3(-1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(xp1, x0, xp2).data(), equalsVector(HVector3(-1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(xp1, xp2, x0).data(), equalsVector(HVector3(-1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(xp2, x0, xp1).data(), equalsVector(HVector3(-1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(xp2, xp1, x0).data(), equalsVector(HVector3(-1.0f, 0.0f, 0.0f)));

    EXPECT_THAT(getPlaneNormal(x0, yp1, yp2).data(), equalsVector(HVector3(0.0f, -1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(x0, yp2, yp1).data(), equalsVector(HVector3(0.0f, -1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(yp1, x0, yp2).data(), equalsVector(HVector3(0.0f, -1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(yp1, yp2, x0).data(), equalsVector(HVector3(0.0f, -1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(yp2, x0, yp1).data(), equalsVector(HVector3(0.0f, -1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(yp2, yp1, x0).data(), equalsVector(HVector3(0.0f, -1.0f, 0.0f)));

    EXPECT_THAT(getPlaneNormal(x0, zp1, zp2).data(), equalsVector(HVector3(0.0f, 0.0f, -1.0f)));
    EXPECT_THAT(getPlaneNormal(x0, zp2, zp1).data(), equalsVector(HVector3(0.0f, 0.0f, -1.0f)));
    EXPECT_THAT(getPlaneNormal(zp1, x0, zp2).data(), equalsVector(HVector3(0.0f, 0.0f, -1.0f)));
    EXPECT_THAT(getPlaneNormal(zp1, zp2, x0).data(), equalsVector(HVector3(0.0f, 0.0f, -1.0f)));
    EXPECT_THAT(getPlaneNormal(zp2, x0, zp1).data(), equalsVector(HVector3(0.0f, 0.0f, -1.0f)));
    EXPECT_THAT(getPlaneNormal(zp2, zp1, x0).data(), equalsVector(HVector3(0.0f, 0.0f, -1.0f)));
  }

  {
    HPoint3 x0(0.0f, 0.0f, 0.0f);

    HPoint3 xp1(0.0f, 1.0f, 0.0f);
    HPoint3 xp2(0.0f, 0.0f, 1.0f);

    HPoint3 yp1(-1.0f, 0.0f, 0.0f);
    HPoint3 yp2(0.0f, 0.0f, -1.0f);

    HPoint3 zp1(0.0f, 2.0f, 0.0f);
    HPoint3 zp2(4.0f, 0.0f, 0.0f);

    EXPECT_THAT(getPlaneNormal(x0, xp1, xp2).data(), equalsVector(HVector3(1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(x0, xp2, xp1).data(), equalsVector(HVector3(1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(xp1, x0, xp2).data(), equalsVector(HVector3(1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(xp1, xp2, x0).data(), equalsVector(HVector3(1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(xp2, x0, xp1).data(), equalsVector(HVector3(1.0f, 0.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(xp2, xp1, x0).data(), equalsVector(HVector3(1.0f, 0.0f, 0.0f)));

    EXPECT_THAT(getPlaneNormal(x0, yp1, yp2).data(), equalsVector(HVector3(0.0f, 1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(x0, yp2, yp1).data(), equalsVector(HVector3(0.0f, 1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(yp1, x0, yp2).data(), equalsVector(HVector3(0.0f, 1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(yp1, yp2, x0).data(), equalsVector(HVector3(0.0f, 1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(yp2, x0, yp1).data(), equalsVector(HVector3(0.0f, 1.0f, 0.0f)));
    EXPECT_THAT(getPlaneNormal(yp2, yp1, x0).data(), equalsVector(HVector3(0.0f, 1.0f, 0.0f)));

    EXPECT_THAT(getPlaneNormal(x0, zp1, zp2).data(), equalsVector(HVector3(0.0f, 0.0f, 1.0f)));
    EXPECT_THAT(getPlaneNormal(x0, zp2, zp1).data(), equalsVector(HVector3(0.0f, 0.0f, 1.0f)));
    EXPECT_THAT(getPlaneNormal(zp1, x0, zp2).data(), equalsVector(HVector3(0.0f, 0.0f, 1.0f)));
    EXPECT_THAT(getPlaneNormal(zp1, zp2, x0).data(), equalsVector(HVector3(0.0f, 0.0f, 1.0f)));
    EXPECT_THAT(getPlaneNormal(zp2, x0, zp1).data(), equalsVector(HVector3(0.0f, 0.0f, 1.0f)));
    EXPECT_THAT(getPlaneNormal(zp2, zp1, x0).data(), equalsVector(HVector3(0.0f, 0.0f, 1.0f)));
  }
}

TEST_F(HomographyTest, TestHomographForPlane) {
  HMatrix camDelta = HMatrixGen::translation(1.0f, -2.0f, 3.0f) * HMatrixGen::yDegrees(30.0);
  HVector3 norm(0.0f, 1.0f, 0.0f);

  float ssq = 2.0f * (.5f * .5f) + 1.0f + 2.0f * (0.866025f * 0.866025f) + 2.0f * (3.0f * 3.0f);
  float is = 1.0f / std::sqrt(ssq);

  HMatrix expected{
    {is * 0.866025f, is * 0.6339745f, is * -0.5000f, 0.0f},
    {0.00000000000f, is * 3.0000000f, 0.00000000000f, 0.0f},
    {is * 0.500000f, is * -3.098076f, is * 0.866025f, 0.0f},
    {0.00000000000f, 0.000000000000f, 0.00000000000f, 1.0f}};

  EXPECT_THAT(homographyForPlane(camDelta, norm).data(), equalsMatrix(expected));
}

TEST_F(HomographyTest, TestExpectedHomographyConsistency) {
  // y-z plane square: 0-3
  // x-z plane pentagon: 4-8
  // x-y plane hexagon: 9-14
  Vector<HPoint3> worldPts = Worlds::axisAlignedPolygonsWorld();

  auto cam1 = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto cam2 = HMatrixGen::translation(7.0, 6.0, 16.0) * HMatrixGen::rotationD(23.0, 199.0, -25.0);

  auto p13 = cam1.inv() * worldPts;
  auto p23 = cam2.inv() * worldPts;

  auto camDelta = egomotion(cam1, cam2);

  auto p1 = flatten<2>(p13);
  auto p2 = flatten<2>(p23);

  {
    HVector3 norm = getScaledPlaneNormal(p13[0], p13[1], p13[2]);
    HMatrix H = homographyForPlane(camDelta, norm);

    auto p2r = flatten<2>(H * extrude<3>(p1));

    for (int i = 0; i < 4; ++i) {
      EXPECT_THAT(p2[i].data(), equalsPoint(p2r[i]));
    }
  }
  {
    HVector3 norm = getScaledPlaneNormal(p13[1], p13[2], p13[3]);
    HMatrix H = homographyForPlane(camDelta, norm);

    auto p2r = flatten<2>(H * extrude<3>(p1));

    for (int i = 0; i < 4; ++i) {
      EXPECT_THAT(p2[i].data(), equalsPoint(p2r[i]));
    }
  }

  {
    HVector3 norm = getScaledPlaneNormal(p13[4], p13[5], p13[6]);
    HMatrix H = homographyForPlane(camDelta, norm);

    auto p2r = flatten<2>(H * extrude<3>(p1));

    for (int i = 4; i < 8; ++i) {
      EXPECT_THAT(p2[i].data(), equalsPoint(p2r[i]));
    }
  }

  {
    HVector3 norm = getScaledPlaneNormal(p13[6], p13[7], p13[8]);
    HMatrix H = homographyForPlane(camDelta, norm);

    auto p2r = flatten<2>(H * extrude<3>(p1));

    for (int i = 4; i < 8; ++i) {
      EXPECT_THAT(p2[i].data(), equalsPoint(p2r[i]));
    }
  }

  {
    HVector3 norm = getScaledPlaneNormal(p13[9], p13[10], p13[11]);
    HMatrix H = homographyForPlane(camDelta, norm);

    auto p2r = flatten<2>(H * extrude<3>(p1));

    for (int i = 9; i < 14; ++i) {
      EXPECT_THAT(p2[i].data(), equalsPoint(p2r[i]));
    }
  }

  {
    HVector3 norm = getScaledPlaneNormal(p13[12], p13[13], p13[14]);
    HMatrix H = homographyForPlane(camDelta, norm);

    auto p2r = flatten<2>(H * extrude<3>(p1));

    for (int i = 9; i < 14; ++i) {
      EXPECT_THAT(p2[i].data(), equalsPoint(p2r[i]));
    }
  }
}

TEST_F(HomographyTest, TestCenteringTransform) {
  // y-z plane square: 0-3
  // x-z plane pentagon: 4-8
  // x-y plane hexagon: 9-14
  Vector<HPoint3> worldPts = Worlds::axisAlignedPolygonsWorld();

  auto cam1 = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto p13 = cam1.inv() * worldPts;
  auto p1 = flatten<2>(p13);

  auto T = centeringTransform(p1);

  auto ps = flatten<2>(T * extrude<3>(p1));

  double sx = 0.0f;
  double sy = 0.0f;
  float ssq = 0.0f;

  for (auto p : ps) {
    sx += p.x();
    sy += p.y();
    ssq += std::sqrt(p.x() * p.x() + p.y() * p.y());
  }

  EXPECT_NEAR(sx, 0.0, 1e-6);
  EXPECT_NEAR(sy, 0.0, 1e-6);
  EXPECT_NEAR(ssq / ps.size(), std::sqrt(2), 1e-6);
}

TEST_F(HomographyTest, TestPlaneFromThreePoints) {
  HVector3 normal;
  float d;
  planeFromThreePoints(
    {HPoint3(0.0f, 0.0f, 0.0f), HPoint3(1.0f, 0.0f, 0.0f), HPoint3(0.0f, 1.0f, 0.0f)}, &normal, &d);

  EXPECT_EQ(normal.x(), 0.0f);
  EXPECT_EQ(normal.y(), 0.0f);
  EXPECT_EQ(normal.z(), 1.0f);
}

TEST_F(HomographyTest, TestTriangulatePointsOnGround) {
  Vector<HPoint3> worldPts = Worlds::axisAlignedPolygonsWorld();
  auto cam1 = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto p13 = cam1.inv() * worldPts;
  auto p1 = flatten<2>(p13);
  Vector<HPoint2> groundPts{p1[4], p1[5], p1[6], p1[7], p1[8]};

  auto reconstruction = triangulatePointsOnGround(cam1, 0.0f, groundPts);

  EXPECT_THAT(worldPts[4].data(), equalsPoint(reconstruction[0]));
  EXPECT_THAT(worldPts[5].data(), equalsPoint(reconstruction[1]));
  EXPECT_THAT(worldPts[6].data(), equalsPoint(reconstruction[2]));
  EXPECT_THAT(worldPts[7].data(), equalsPoint(reconstruction[3]));
  EXPECT_THAT(worldPts[8].data(), equalsPoint(reconstruction[4]));
}

TEST_F(HomographyTest, TestGroundPlaneRotation) {
  {
    // Mixed (x, y, z) rotation.
    auto cam =
      HMatrixGen::yDegrees(190.0) * HMatrixGen::xDegrees(12.0) * HMatrixGen::zDegrees(-15.0);
    auto gpr = groundPlaneRotation(Quaternion::fromHMatrix(cam)).toRotationMat();
    auto expected = HMatrixGen::yDegrees(190.0);
    EXPECT_THAT(gpr.data(), equalsMatrix(expected));
  }

  {
    // Only (x, z) rotation, no y rotation.
    auto cam = HMatrixGen::xDegrees(12.0) * HMatrixGen::zDegrees(-15.0);
    auto gpr = groundPlaneRotation(Quaternion::fromHMatrix(cam)).toRotationMat();
    auto expected = HMatrixGen::i();
    EXPECT_THAT(gpr.data(), equalsMatrix(expected));
  }

  {
    // Only y rotation, no (x, z) rotation.
    auto cam = HMatrixGen::yDegrees(-45.0);
    auto gpr = groundPlaneRotation(Quaternion::fromHMatrix(cam)).toRotationMat();
    auto expected = HMatrixGen::yDegrees(-45.0);
    EXPECT_THAT(gpr.data(), equalsMatrix(expected));
  }

  {
    // Rotate about x so that we're facing backwards.
    auto cam = HMatrixGen::xDegrees(180.0);
    auto gpr = groundPlaneRotation(Quaternion::fromHMatrix(cam)).toRotationMat();
    auto expected = HMatrixGen::yDegrees(180.0);
    EXPECT_THAT(gpr.data(), equalsMatrix(expected));
  }

  {
    // Rotate about x so that we're facing down.
    auto cam = HMatrixGen::xDegrees(90.0);
    auto gpr = groundPlaneRotation(Quaternion::fromHMatrix(cam)).toRotationMat();
    auto expected = HMatrixGen::i();
    EXPECT_THAT(gpr.data(), equalsMatrix(expected));
  }

  {
    // Rotate about x so that we're facing down, and then rotate in plane.
    auto cam = HMatrixGen::yDegrees(-45.0) * HMatrixGen::xDegrees(90.0);
    auto gpr = groundPlaneRotation(Quaternion::fromHMatrix(cam)).toRotationMat();
    auto expected = HMatrixGen::yDegrees(-45.0);
    EXPECT_THAT(gpr.data(), equalsMatrix(expected));
  }

  {
    // Rotate about x so that we're facing up.
    auto cam = HMatrixGen::xDegrees(-90.0);
    auto gpr = groundPlaneRotation(Quaternion::fromHMatrix(cam)).toRotationMat();
    auto expected = HMatrixGen::yDegrees(180.0);
    EXPECT_THAT(gpr.data(), equalsMatrix(expected));
  }
}

TEST_F(HomographyTest, TestIsPureCameraRotation) {
  auto cam1 = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto cam2 = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(23.0, 199.0, -25.0);
  auto cam3 = HMatrixGen::translation(7.0, 6.0, 16.0) * HMatrixGen::rotationD(23.0, 199.0, -25.0);

  EXPECT_EQ(true, isPureCameraRotation(cam1, cam2));
  EXPECT_EQ(false, isPureCameraRotation(cam1, cam3));
}

}  // namespace c8
