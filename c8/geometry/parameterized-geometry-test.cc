// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":parameterized-geometry",
    "//c8/geometry:line",
    "//c8/string:format",
    "//c8:c8-log",
    "//c8:hmatrix",
    "//c8:random-numbers",
    "//c8/geometry:intrinsics",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x989ad755);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/c8-log.h"
#include "c8/geometry/intrinsics.h"
#include "c8/geometry/line.h"
#include "c8/geometry/parameterized-geometry.h"
#include "c8/hmatrix.h"
#include "c8/random-numbers.h"
#include "c8/string/format.h"

using testing::Eq;
using testing::FloatNear;
using testing::Pointwise;

namespace c8 {

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsMatrix(const HMatrix &matrix, float threshold = 1e-6) {
  return Pointwise(AreWithin(threshold), matrix.data());
}

decltype(auto) equalsPoint(const HPoint2 &point) {
  return testing::Pointwise(AreWithin(0.0001), point.data());
}

decltype(auto) equalsPixel(const HPoint2 &point) {
  return testing::Pointwise(AreWithin(1.0), point.data());
}

decltype(auto) equalsPoint(const HPoint3 &point, float threshold = 0.0001) {
  return testing::Pointwise(AreWithin(threshold), point.data());
}

CurvyImageGeometry geom, fullGeom;
CurvyImageGeometry coneGeom, coneFullGeom;
class ParameterizedGeometryTest : public ::testing::Test {
public:
    static void SetUpTestSuite() {
      curvyForTarget(480, 640, {0.6}, &geom, &fullGeom);
      curvyForTarget(480, 640, {0.6, false, {}, 1.42}, &coneGeom, &coneFullGeom);
    }
};

// Get a consistent test intrinsic matrix.
static c8_PixelPinholeCameraModel testK() {
  return Intrinsics::getCameraIntrinsics(DeviceInfos::GOOGLE_PIXEL3);
}

Vector<HPoint3> getPointsOnCurvy(CurvyImageGeometry geom) {
  float curvyHalfHeight = geom.height / 2;
  float radiusAt0 = geom.isCone ? (geom.radius + geom.radiusBottom) / 2 : geom.radius;
  return Vector<HPoint3>{// right points
                         {geom.radiusBottom, -curvyHalfHeight, 0.f},
                         {radiusAt0, 0.f, 0.f},
                         {geom.radius, curvyHalfHeight, 0.f},
                         // left points
                         {-geom.radiusBottom, -curvyHalfHeight, 0.f},
                         {-radiusAt0, 0.f, 0.f},
                         {-geom.radius, curvyHalfHeight, 0.f},
                         // back points
                         {0.f, -curvyHalfHeight, geom.radiusBottom},
                         {0.f, 0.f, radiusAt0},
                         {0.f, curvyHalfHeight, geom.radius},
                         // front points
                         {0.f, -curvyHalfHeight, -geom.radiusBottom},
                         {0.f, 0.f, -radiusAt0},
                         {0.f, curvyHalfHeight, -geom.radius}};
}

void getRandomPointsOnCurvy(CurvyImageGeometry geom, int numPoints, Vector<HPoint3> *points, Vector<HVector3> *normals) {
  points->reserve(numPoints);
  normals->reserve(numPoints);
  points->clear();
  normals->clear();
  // float halfConeTheta = computeTheta(geom) / 2;
  RandomNumbers randomNum;
  for (size_t i = 0; i < numPoints; i++)
  {
    float y = randomNum.nextUniform32f() - 0.5;
    float radiusAtY = geom.radius * (y + 0.5) * geom.radiusBottom * (y - 0.5);
    float theta = randomNum.nextUniform32f() * 2 * M_PI;
    points->emplace_back(-radiusAtY * std::sin(theta), y, radiusAtY * std::cos(theta));

    // float normalY = ((geom.radius < geom.radiusBottom) ? 1.f : -1.f) * radiusAtY * tan(halfConeTheta);
    normals->push_back(HVector3 {-radiusAtY * std::sin(theta), y, radiusAtY * std::cos(theta)}.unit());
  }

}

void generateRays(
  const Vector<HPoint3> &pts,
  const HMatrix &cameraPose,
  Vector<HPoint2> *rays,
  Vector<HPoint3> *worldPts) {
  *worldPts = cameraPose.inv() * pts;
  *rays = flatten<2>(*worldPts);
}

TEST_F(ParameterizedGeometryTest, constructSimpleGeometryFromSpec) {
  CurvyImageGeometry equivalentGeom {0.198943, 1.0, {0.2, 0.8f, 0.f, 1.f}, 640, 480};
  EXPECT_NEAR(geom.radius, equivalentGeom.radius, 1e-6);
  EXPECT_NEAR(geom.height, equivalentGeom.height, 1e-6);
  EXPECT_EQ(geom.srcRows, equivalentGeom.srcRows);
  EXPECT_EQ(geom.srcCols, equivalentGeom.srcCols);
  EXPECT_NEAR(geom.activationRegion.left, equivalentGeom.activationRegion.left, 1e-6);
  EXPECT_NEAR(geom.activationRegion.right, equivalentGeom.activationRegion.right, 1e-6);
  EXPECT_NEAR(geom.activationRegion.top, equivalentGeom.activationRegion.top, 1e-6);
  EXPECT_NEAR(geom.activationRegion.bottom, equivalentGeom.activationRegion.bottom, 1e-6);
}

TEST_F(ParameterizedGeometryTest, constructCroppedGeometryFromSpecSimple) {
  // we crop at half the size right in the middle of the target
  // since the arc stays the same but the tracked curvy is half as big, the radius grows twice as big
  CurvyImageGeometry equivalentGeom {0.397887, 1.0, {0.35, 0.65f, 0.25f, 0.75f}, 320, 240};
  CurvyImageGeometry specGeom, specFullGeom;
  curvyForTarget(240, 320, {0.6, false, {2, 2, 0.25, 0.25}}, &specGeom, &specFullGeom);
  EXPECT_NEAR(specGeom.radius, equivalentGeom.radius, 1e-6);
  EXPECT_NEAR(specGeom.height, equivalentGeom.height, 1e-6);
  EXPECT_EQ(specGeom.srcRows, equivalentGeom.srcRows);
  EXPECT_EQ(specGeom.srcCols, equivalentGeom.srcCols);
  EXPECT_NEAR(specGeom.activationRegion.left, equivalentGeom.activationRegion.left, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.right, equivalentGeom.activationRegion.right, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.top, equivalentGeom.activationRegion.top, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.bottom, equivalentGeom.activationRegion.bottom, 1e-6);
  EXPECT_EQ(specGeom.isCone, equivalentGeom.isCone);

  EXPECT_NEAR(specFullGeom.radius, specGeom.radius, 1e-6) << "Cylinder radius is always the same";
  EXPECT_NEAR(specFullGeom.height, specGeom.height * 2, 1e-6) << "Cylinder full height is scaled inverse to activation region";
  EXPECT_NEAR(specFullGeom.activationRegion.left, 0.2, 1e-6);
  EXPECT_NEAR(specFullGeom.activationRegion.right, 0.8, 1e-6);
  EXPECT_NEAR(specFullGeom.activationRegion.top, 0.f, 1e-6);
  EXPECT_NEAR(specFullGeom.activationRegion.bottom, 1.f, 1e-6);
}

TEST_F(ParameterizedGeometryTest, constructCroppedGeometryFromSpec) {
  CurvyImageGeometry specGeom, specFullGeom;
  curvyForTarget(400, 533, {0.6, false, {1.2, 1.2, 0.1, 0.1}}, &specGeom, &specFullGeom);
  CurvyImageGeometry equivalentGeom {0.238881, 1.0, {0.26, 0.76f, 0.1f, 0.933333f}, 533, 400};
  EXPECT_NEAR(specGeom.radius, equivalentGeom.radius, 1e-6);
  EXPECT_NEAR(specGeom.height, equivalentGeom.height, 1e-6);
  EXPECT_EQ(specGeom.srcRows, equivalentGeom.srcRows);
  EXPECT_EQ(specGeom.srcCols, equivalentGeom.srcCols);
  EXPECT_NEAR(specGeom.activationRegion.left, equivalentGeom.activationRegion.left, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.right, equivalentGeom.activationRegion.right, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.top, equivalentGeom.activationRegion.top, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.bottom, equivalentGeom.activationRegion.bottom, 1e-6);
  EXPECT_EQ(specGeom.isCone, equivalentGeom.isCone);
}

TEST_F(ParameterizedGeometryTest, constructFullConeFromSpec) {
  float base = 27.6 / 20.2; // cup circumference top / cup circumference bottom
  CurvyImageGeometry specGeom, specFullGeom;
  curvyForTarget(1600, 654, {1.0, false, {1.0, 1.0, 0., 0.}, base}, &specGeom, &specFullGeom);
  float radiusTop = 0.391509f;
  CurvyImageGeometry equivalentGeom {radiusTop, 1.0, {0.f, 1.f, 0.f, 1.0f}, 654, 1600, true, radiusTop / base};
  EXPECT_NEAR(specGeom.radius, equivalentGeom.radius, 1e-3);
  EXPECT_NEAR(specGeom.height, equivalentGeom.height, 1e-6);
  EXPECT_EQ(specGeom.srcRows, equivalentGeom.srcRows);
  EXPECT_EQ(specGeom.srcCols, equivalentGeom.srcCols);
  EXPECT_NEAR(specGeom.activationRegion.left, equivalentGeom.activationRegion.left, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.right, equivalentGeom.activationRegion.right, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.top, equivalentGeom.activationRegion.top, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.bottom, equivalentGeom.activationRegion.bottom, 1e-6);
  EXPECT_EQ(specGeom.isCone, equivalentGeom.isCone);
  EXPECT_NEAR(specGeom.radiusBottom, equivalentGeom.radiusBottom, 1e-3);
}

TEST_F(ParameterizedGeometryTest, constructCroppedConeFromSpec) {
  float base = 27.6 / 20.2; // cup circumference top / cup circumference bottom
  CurvyImageGeometry specGeom, specFullGeom;
  curvyForTarget(490, 654, {1.0, false, {3.265306, 1.0, 0.6, 0.}, base}, &specGeom, &specFullGeom);
  float radiusTopNoCrop = 0.391509f; // this value should be the same as the test in constructFullConeFromSpec
  float radiusBottomNoCrop = radiusTopNoCrop / base;
  CurvyImageGeometry equivalentGeom {radiusTopNoCrop, 1.0, {0.6f, 0.90625f, 0.f, 1.0f}, 654, 490, true, radiusBottomNoCrop};
  EXPECT_NEAR(specGeom.radius, equivalentGeom.radius, 1e-3);
  EXPECT_NEAR(specGeom.height, equivalentGeom.height, 1e-6);
  EXPECT_EQ(specGeom.srcRows, equivalentGeom.srcRows);
  EXPECT_EQ(specGeom.srcCols, equivalentGeom.srcCols);
  EXPECT_NEAR(specGeom.activationRegion.left, equivalentGeom.activationRegion.left, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.right, equivalentGeom.activationRegion.right, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.top, equivalentGeom.activationRegion.top, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.bottom, equivalentGeom.activationRegion.bottom, 1e-6);
  EXPECT_EQ(specGeom.isCone, equivalentGeom.isCone);
  EXPECT_NEAR(specGeom.radiusBottom, equivalentGeom.radiusBottom, 1e-3);

  // we construct the same cropped cone but this time with vertical crop in the middle
  CurvyImageGeometry specGeom2, specFullGeom2;
  curvyForTarget(490, 654/2, {1.0, false, {3.265306, 2.0, 0.6, 0.25}, base}, &specGeom2, &specFullGeom2);
  // radius is twice as big since the crop height is twice as small
  float radiusTop = 2 * (radiusTopNoCrop * 0.75 + 0.25 * radiusBottomNoCrop);
  float radiusBottom = 2 * (radiusTopNoCrop * 0.25 + 0.75 * radiusBottomNoCrop);
  CurvyImageGeometry equivalentGeom2 {radiusTop, 1.0, {0.6f, 0.90625f, 0.25f, 0.75f}, 654/2, 490, true, radiusBottom};
  EXPECT_NEAR(specGeom2.radius, equivalentGeom2.radius, 1e-3);
  EXPECT_NEAR(specGeom2.height, equivalentGeom2.height, 1e-6);
  EXPECT_EQ(specGeom2.srcRows, equivalentGeom2.srcRows);
  EXPECT_EQ(specGeom2.srcCols, equivalentGeom2.srcCols);
  EXPECT_NEAR(specGeom2.activationRegion.left, equivalentGeom2.activationRegion.left, 1e-6);
  EXPECT_NEAR(specGeom2.activationRegion.right, equivalentGeom2.activationRegion.right, 1e-6);
  EXPECT_NEAR(specGeom2.activationRegion.top, equivalentGeom2.activationRegion.top, 1e-6);
  EXPECT_NEAR(specGeom2.activationRegion.bottom, equivalentGeom2.activationRegion.bottom, 1e-6);
  EXPECT_EQ(specGeom2.isCone, equivalentGeom2.isCone);
  EXPECT_NEAR(specGeom2.radiusBottom, equivalentGeom2.radiusBottom, 1e-3);
}

TEST_F(ParameterizedGeometryTest, constructCroppedConeFromSpecSimple) {
  CurvyImageGeometry specGeom, specFullGeom;
  curvyForTarget(480, 640, {1.0, false, {2.0, 2.0, 0.25, 0.25}, 1.5}, &specGeom, &specFullGeom);

  EXPECT_NEAR(specGeom.radius, 0.219011f, 1e-3);
  EXPECT_NEAR(specGeom.height, 1.f, 1e-6);
  EXPECT_EQ(specGeom.srcRows, 640);
  EXPECT_EQ(specGeom.srcCols, 480);
  EXPECT_NEAR(specGeom.activationRegion.left, 0.25, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.right, 0.75, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.top, 0.25, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.bottom, 0.75, 1e-6);
  EXPECT_TRUE(specGeom.isCone);
  EXPECT_NEAR(specGeom.radiusBottom, 0.179191f, 1e-3);

  EXPECT_NEAR(specFullGeom.radius, 0.238921f, 1e-3);
  EXPECT_NEAR(specFullGeom.height, 2.f, 1e-6);
  EXPECT_EQ(specFullGeom.srcRows, 640 * 2);
  EXPECT_EQ(specFullGeom.srcCols, 480 * 2);
  EXPECT_NEAR(specFullGeom.activationRegion.left, 0., 1e-6);
  EXPECT_NEAR(specFullGeom.activationRegion.right, 1., 1e-6);
  EXPECT_NEAR(specFullGeom.activationRegion.top, 0., 1e-6);
  EXPECT_NEAR(specFullGeom.activationRegion.bottom, 1., 1e-6);
  EXPECT_TRUE(specGeom.isCone);
  EXPECT_NEAR(specFullGeom.radiusBottom, 0.159281f, 1e-3);
}

TEST_F(ParameterizedGeometryTest, constructCroppedFezFromSpecSimple) {
  // this is the same as constructCroppedConeFromSpecSimple just up side down
  CurvyImageGeometry specGeom, specFullGeom;
  curvyForTarget(480, 640, {1.0, false, {2.0, 2.0, 0.25, 0.25}, 1.f / 1.5}, &specGeom, &specFullGeom);

  EXPECT_NEAR(specGeom.radiusBottom, 0.219011f, 1e-3);
  EXPECT_NEAR(specGeom.height, 1.f, 1e-6);
  EXPECT_EQ(specGeom.srcRows, 640);
  EXPECT_EQ(specGeom.srcCols, 480);
  EXPECT_NEAR(specGeom.activationRegion.left, 0.25, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.right, 0.75, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.top, 0.25, 1e-6);
  EXPECT_NEAR(specGeom.activationRegion.bottom, 0.75, 1e-6);
  EXPECT_TRUE(specGeom.isCone);
  EXPECT_NEAR(specGeom.radius, 0.179191f, 1e-3);

  EXPECT_NEAR(specFullGeom.radiusBottom, 0.238921f, 1e-3);
  EXPECT_NEAR(specFullGeom.height, 2.f, 1e-6);
  EXPECT_EQ(specFullGeom.srcRows, 640 * 2);
  EXPECT_EQ(specFullGeom.srcCols, 480 * 2);
  EXPECT_NEAR(specFullGeom.activationRegion.left, 0., 1e-6);
  EXPECT_NEAR(specFullGeom.activationRegion.right, 1., 1e-6);
  EXPECT_NEAR(specFullGeom.activationRegion.top, 0., 1e-6);
  EXPECT_NEAR(specFullGeom.activationRegion.bottom, 1., 1e-6);
  EXPECT_TRUE(specGeom.isCone);
  EXPECT_NEAR(specFullGeom.radius, 0.159281f, 1e-3);
}

TEST_F(ParameterizedGeometryTest, cameraRaysToTargetWorldNoMove) {
  HMatrix cameraPose = HMatrixGen::i();

  Vector<HPoint3> pts = getPointsOnCurvy(geom);
  Vector<HPoint2> rays;
  Vector<HPoint3> worldPts;
  generateRays(pts, cameraPose, &rays, &worldPts);

  auto recoveredWorldPts = cameraRaysToTargetWorld(geom, rays, cameraPose);
  // Note that the recovered world points is correct as long as it on the same ray
  auto recoveredRays = flatten<2>(recoveredWorldPts);

  // without movement, only the back points are visible
  for (int i = 6; i < 9; i++) {
    EXPECT_THAT(recoveredWorldPts[i].data(), equalsPoint(pts[i])) << format(
      "Comparing back world pt %f %f %f to recovered pt %f %f %f to model pt %f %f %f, recovered "
      "ray %f %f to ray %f %f",
      worldPts[i].x(),
      worldPts[i].y(),
      worldPts[i].z(),
      recoveredWorldPts[i].x(),
      recoveredWorldPts[i].y(),
      recoveredWorldPts[i].z(),
      pts[i].x(),
      pts[i].y(),
      pts[i].z(),
      recoveredRays[i].x(),
      recoveredRays[i].y(),
      rays[i].x(),
      rays[i].y());
    ;
  }
}

TEST_F(ParameterizedGeometryTest, cameraRaysToTargetWorldHeightClipping) {
  HMatrix cameraPose = HMatrixGen::i();

  Vector<HPoint3> allPts = getPointsOnCurvy(geom);
  Vector<HPoint3> pts(6);
  // Without movement, only the back points are visible (6, 7, 8). Copy over those points and
  // also create artificial points that are too low / high.
  Vector<float> errs{-.01f, 300.f, .01f};
  for (int i = 0; i < 3; i++) {
    auto pt = allPts[6 + i];
    // These points are on the edges of the cylinder and should be ok.
    pts[i] = pt;
    // We adjust these points to be out of the height boundary.
    pts[i + 3] = {pt.x(), pt.y() + errs[i], pt.z()};
  }

  Vector<HPoint2> rays;
  Vector<HPoint3> worldPts;
  generateRays(pts, cameraPose, &rays, &worldPts);

  auto recoveredWorldPts = cameraRaysToTargetWorld(geom, rays, cameraPose);
  // Note that the recovered world points is correct as long as it on the same ray
  auto recoveredRays = flatten<2>(recoveredWorldPts);
  for (int i = 0; i < 3; i++) {
    EXPECT_THAT(recoveredWorldPts[i].data(), equalsPoint(pts[i])) << format(
      "Comparing back world pt %f %f %f to recovered pt %f %f %f to model pt %f %f %f, recovered "
      "ray %f %f to ray %f %f",
      worldPts[i].x(),
      worldPts[i].y(),
      worldPts[i].z(),
      recoveredWorldPts[i].x(),
      recoveredWorldPts[i].y(),
      recoveredWorldPts[i].z(),
      pts[i].x(),
      pts[i].y(),
      pts[i].z(),
      recoveredRays[i].x(),
      recoveredRays[i].y(),
      rays[i].x(),
      rays[i].y());
    ;
    EXPECT_EQ(recoveredWorldPts[i + 3].x(), -1000.f);
  }
}

TEST_F(ParameterizedGeometryTest, cameraRaysToTargetWorldOnlyRotation) {
  // some points on the curvy model space
  Vector<HPoint3> pts = getPointsOnCurvy(geom);

  HMatrix cameraPose = HMatrixGen::rotationD(45, 35, -12);

  Vector<HPoint2> rays;
  Vector<HPoint3> worldPts;
  generateRays(pts, cameraPose, &rays, &worldPts);

  auto recoveredWorldPts = cameraRaysToTargetWorld(geom, rays, cameraPose);
  auto recoveredRays = flatten<2>(recoveredWorldPts);

  // NOTE: (dat) since there is no translation, we can't tell for sure which point
  // is correct so we allow a bit more flexibility
  for (int i = 0; i < 12; i++) {
    HPoint3 negativePt{-pts[i].x(), -pts[i].y(), -pts[i].z()};
    HPoint3 comparedPts = pts[i];
    // We have to flip to the other side of the curvy if one of the x,y,z component is of
    // the wrong sign ignoring values close to zero.
    if (
      (pts[i].x() * recoveredWorldPts[i].x() < 0 && std::abs(recoveredWorldPts[i].x()) > 1E-6)
      || (pts[i].y() * recoveredWorldPts[i].y() < 0 && std::abs(recoveredWorldPts[i].y()) > 1E-6)
      || (pts[i].z() * recoveredWorldPts[i].z() < 0 && std::abs(recoveredWorldPts[i].z()) > 1E-6)) {
      comparedPts = negativePt;
    }
    EXPECT_THAT(recoveredWorldPts[i].data(), equalsPoint(comparedPts)) << format(
      "Comparing %d world pt %f %f %f to recovered pt %f %f %f to model pt %f %f %f, recovered "
      "ray %f %f to ray %f %f",
      i,
      worldPts[i].x(),
      worldPts[i].y(),
      worldPts[i].z(),
      recoveredWorldPts[i].x(),
      recoveredWorldPts[i].y(),
      recoveredWorldPts[i].z(),
      comparedPts.x(),
      comparedPts.y(),
      comparedPts.z(),
      recoveredRays[i].x(),
      recoveredRays[i].y(),
      rays[i].x(),
      rays[i].y());
  }
}

TEST_F(ParameterizedGeometryTest, cameraRaysToTargetWorldOnlyTranslation) {
  // some points on the curvy model space
  Vector<HPoint3> pts = getPointsOnCurvy(geom);

  HMatrix cameraPose = HMatrixGen::translation(-3, -5, -6);

  Vector<HPoint2> rays;
  Vector<HPoint3> worldPts;
  generateRays(pts, cameraPose, &rays, &worldPts);

  auto recoveredWorldPts = cameraRaysToTargetWorld(geom, rays, cameraPose);
  auto recoveredRays = flatten<2>(recoveredWorldPts);

  // Since the points are to the right above and in front
  // we can only see points on the left + front of the curvy
  for (int i = 3; i < 6; i++) {
    EXPECT_THAT(recoveredWorldPts[i].data(), equalsPoint(pts[i])) << format(
      "Comparing left side world pt %f %f %f to recovered pt %f %f %f to model pt %f %f %f, "
      "recovered "
      "ray %f %f to ray %f %f",
      worldPts[i].x(),
      worldPts[i].y(),
      worldPts[i].z(),
      recoveredWorldPts[i].x(),
      recoveredWorldPts[i].y(),
      recoveredWorldPts[i].z(),
      pts[i].x(),
      pts[i].y(),
      pts[i].z(),
      recoveredRays[i].x(),
      recoveredRays[i].y(),
      rays[i].x(),
      rays[i].y());
    ;
  }

  for (int i = 9; i < 12; i++) {
    EXPECT_THAT(recoveredWorldPts[i].data(), equalsPoint(pts[i])) << format(
      "Comparing front world pt %f %f %f to recovered pt %f %f %f to model pt %f %f %f, recovered "
      "ray %f %f to ray %f %f",
      worldPts[i].x(),
      worldPts[i].y(),
      worldPts[i].z(),
      recoveredWorldPts[i].x(),
      recoveredWorldPts[i].y(),
      recoveredWorldPts[i].z(),
      pts[i].x(),
      pts[i].y(),
      pts[i].z(),
      recoveredRays[i].x(),
      recoveredRays[i].y(),
      rays[i].x(),
      rays[i].y());
    ;
  }
}

TEST_F(ParameterizedGeometryTest, cameraRaysToTargetWorldBothRotationAndTranslation) {
  // some points on the curvy model space
  Vector<HPoint3> pts = getPointsOnCurvy(geom);

  HMatrix cameraPose = HMatrixGen::translation(-3, 5, 6) * HMatrixGen::rotationD(15, -25, -30);

  Vector<HPoint2> rays;
  Vector<HPoint3> worldPts;
  generateRays(pts, cameraPose, &rays, &worldPts);

  auto recoveredWorldPts = cameraRaysToTargetWorld(geom, rays, cameraPose);
  auto recoveredRays = flatten<2>(recoveredWorldPts);

  // Since the points are to the left above and in front
  // we can only see points on the right + front of the curvy
  for (int i = 3; i < 6; i++) {
    EXPECT_THAT(recoveredWorldPts[i].data(), equalsPoint(pts[i])) << format(
      "Comparing right side pt %f %f %f to recovered pt %f %f %f to model pt %f %f %f, recovered "
      "ray %f %f to ray %f %f",
      worldPts[i].x(),
      worldPts[i].y(),
      worldPts[i].z(),
      recoveredWorldPts[i].x(),
      recoveredWorldPts[i].y(),
      recoveredWorldPts[i].z(),
      pts[i].x(),
      pts[i].y(),
      pts[i].z(),
      recoveredRays[i].x(),
      recoveredRays[i].y(),
      rays[i].x(),
      rays[i].y());
    ;
  }

  for (int i = 6; i < 9; i++) {
    EXPECT_THAT(recoveredWorldPts[i].data(), equalsPoint(pts[i])) << format(
      "Comparing front world pt %f %f %f to recovered pt %f %f %f to model pt %f %f %f, recovered "
      "ray %f %f to ray %f %f",
      worldPts[i].x(),
      worldPts[i].y(),
      worldPts[i].z(),
      recoveredWorldPts[i].x(),
      recoveredWorldPts[i].y(),
      recoveredWorldPts[i].z(),
      pts[i].x(),
      pts[i].y(),
      pts[i].z(),
      recoveredRays[i].x(),
      recoveredRays[i].y(),
      rays[i].x(),
      rays[i].y());
    ;
  }
}

TEST_F(ParameterizedGeometryTest, cameraRaysToTargetWorldBothRotationAndTranslationCone) {
  // some points on the curvy model space
  Vector<HPoint3> pts = getPointsOnCurvy(coneGeom);

  HMatrix cameraPose = HMatrixGen::translation(-3, 5, 6) * HMatrixGen::rotationD(15, -25, -30);

  Vector<HPoint2> rays;
  Vector<HPoint3> worldPts;
  generateRays(pts, cameraPose, &rays, &worldPts);

  auto recoveredWorldPts = cameraRaysToTargetWorld(coneGeom, rays, cameraPose);
  auto recoveredRays = flatten<2>(recoveredWorldPts);

  // Since the points are to the left above and in front
  // we can only see points on the right + front of the curvy
  for (int i = 3; i < 6; i++) {
    EXPECT_THAT(recoveredWorldPts[i].data(), equalsPoint(pts[i])) << format(
      "Comparing right side pt %s to recovered pt %s to model pt %s, recovered "
      "ray %s to ray %s",
      worldPts[i].toString().c_str(),
      recoveredWorldPts[i].toString().c_str(),
      pts[i].toString().c_str(),
      recoveredRays[i].toString().c_str(),
      rays[i].toString().c_str()
    );
  }

  for (int i = 6; i < 9; i++) {
    EXPECT_THAT(recoveredWorldPts[i].data(), equalsPoint(pts[i])) << format(
      "Comparing right side pt %s to recovered pt %s to model pt %s, recovered "
      "ray %s to ray %s",
      worldPts[i].toString().c_str(),
      recoveredWorldPts[i].toString().c_str(),
      pts[i].toString().c_str(),
      recoveredRays[i].toString().c_str(),
      rays[i].toString().c_str()
    );
  }
}

TEST_F(ParameterizedGeometryTest, mapToThenFromGeometry) {
  // transformation between the pixel space and the model space are invertible.
  const int NUM_POINTS = 10;
  Vector<HPoint2> randomImagePts;
  randomImagePts.reserve(NUM_POINTS);
  RandomNumbers randNum;
  for (int i = 0; i < NUM_POINTS; i++) {
    randomImagePts.emplace_back(
      static_cast<float>(randNum.nextUniformInt(0, geom.srcCols)),
      static_cast<float>(randNum.nextUniformInt(0, geom.srcRows)));
  }

  Vector<HPoint3> modelPts;
  Vector<HVector3> modelNormals;
  mapToGeometry(geom, randomImagePts, &modelPts, &modelNormals);

  Vector<HPoint2> recoveredPixels = mapFromGeometry(geom, modelPts);

  for (int i = 0; i < NUM_POINTS; i++) {
    // recovered points are within 1 pixel
    EXPECT_THAT(recoveredPixels[i].data(), equalsPixel(randomImagePts[i]));
  }
}

TEST_F(ParameterizedGeometryTest, mapToGeometryForCone) {
  // transformation between the pixel space and the model space are invertible.
  const int NUM_POINTS = 10;
  Vector<HPoint2> randomImagePts;
  randomImagePts.reserve(NUM_POINTS);
  RandomNumbers randNum;
  for (int i = 0; i < NUM_POINTS; i++) {
    randomImagePts.emplace_back(
      static_cast<float>(randNum.nextUniformInt(0, coneGeom.srcCols)),
      static_cast<float>(randNum.nextUniformInt(0, coneGeom.srcRows)));
  }

  Vector<HPoint3> modelPts;
  Vector<HVector3> modelNormals;
  mapToGeometry(coneGeom, randomImagePts, &modelPts, &modelNormals);

  Vector<HPoint2> recoveredPixels = mapFromGeometry(coneGeom, modelPts);

  for (int i = 0; i < NUM_POINTS; i++) {
    // recovered points are within 1 pixel
    EXPECT_THAT(recoveredPixels[i].data(), equalsPixel(randomImagePts[i]));
  }
}

TEST_F(ParameterizedGeometryTest, mapToGeometryNormalCone) {
  const int NUM_POINTS = 10;
  Vector<HPoint2> randomImagePts, randomImagePtsTop, randomImagePtsBottom;
  randomImagePts.reserve(NUM_POINTS);
  // The same x but at the top radius and bottom radius
  randomImagePtsTop.reserve(NUM_POINTS);
  randomImagePtsBottom.reserve(NUM_POINTS);
  RandomNumbers randNum;
  for (int i = 0; i < NUM_POINTS; i++) {
    float x = randNum.nextUniformInt(0, coneGeom.srcCols);
    float y = randNum.nextUniformInt(0, coneGeom.srcRows);
    randomImagePts.emplace_back(x, y);
    randomImagePtsTop.emplace_back(x, 0.f);
    randomImagePtsBottom.emplace_back(x, static_cast<float>(coneGeom.srcRows - 1));
  }

  Vector<HPoint3> modelPts, modelPtsTop, modelPtsBottom;
  Vector<HVector3> modelNormals;
  mapToGeometry(coneGeom, randomImagePts, &modelPts, &modelNormals);
  mapToGeometryPoints(coneGeom, randomImagePtsTop, &modelPtsTop);
  mapToGeometryPoints(coneGeom, randomImagePtsBottom, &modelPtsBottom);

  // points that we recovered have to lie on the line connecting the top and the bottom
  for (int i = 0; i < NUM_POINTS; i++) {
    EXPECT_TRUE(pointOnLineBetween(modelPtsBottom[i], modelPts[i], modelPtsTop[i]));
  }

  // normals of the points have to be perpendicular to the point itself
  for (int i = 0; i < NUM_POINTS; i++) {
    EXPECT_TRUE(linesPerpendicular(modelNormals[i], line(modelPtsBottom[i], modelPtsTop[i])))
      << c8::format("bottom pt %f, %f, %f top pt %f %f %f normals %f %f %f",
      modelPtsBottom[i].x(), modelPtsBottom[i].y(), modelPtsBottom[i].z(),
      modelPtsTop[i].x(), modelPtsTop[i].y(), modelPtsTop[i].z(),
      modelNormals[i].x(), modelNormals[i].y(), modelNormals[i].z()
      );
  }
}

TEST_F(ParameterizedGeometryTest, mapToGeometryNormalCylinder) {
  const int NUM_POINTS = 10;
  Vector<HPoint2> randomImagePts, randomImagePtsTop, randomImagePtsBottom;
  randomImagePts.reserve(NUM_POINTS);
  // The same x but at the top radius and bottom radius
  randomImagePtsTop.reserve(NUM_POINTS);
  randomImagePtsBottom.reserve(NUM_POINTS);
  RandomNumbers randNum;
  for (int i = 0; i < NUM_POINTS; i++) {
    float x = randNum.nextUniformInt(0, coneGeom.srcCols);
    float y = randNum.nextUniformInt(0, coneGeom.srcRows);
    randomImagePts.emplace_back(x, y);
    randomImagePtsTop.emplace_back(x, 0.f);
    randomImagePtsBottom.emplace_back(x, static_cast<float>(coneGeom.srcRows - 1));
  }

  Vector<HPoint3> modelPts, modelPtsTop, modelPtsBottom;
  Vector<HVector3> modelNormals;
  mapToGeometry(geom, randomImagePts, &modelPts, &modelNormals);
  mapToGeometryPoints(geom, randomImagePtsTop, &modelPtsTop);
  mapToGeometryPoints(geom, randomImagePtsBottom, &modelPtsBottom);

  // points that we recovered have to lie on the line connecting the top and the bottom
  for (int i = 0; i < NUM_POINTS; i++) {
    EXPECT_TRUE(pointOnLineBetween(modelPtsBottom[i], modelPts[i], modelPtsTop[i]));
  }

  // normals of the points have to be perpendicular to the point itself
  for (int i = 0; i < NUM_POINTS; i++) {
    EXPECT_TRUE(linesPerpendicular(modelNormals[i], line(modelPtsBottom[i], modelPtsTop[i])))
      << c8::format("bottom pt %f, %f, %f top pt %f %f %f normals %f %f %f",
      modelPtsBottom[i].x(), modelPtsBottom[i].y(), modelPtsBottom[i].z(),
      modelPtsTop[i].x(), modelPtsTop[i].y(), modelPtsTop[i].z(),
      modelNormals[i].x(), modelNormals[i].y(), modelNormals[i].z()
      );
  }
}

TEST_F(ParameterizedGeometryTest, mapToGeometry) {
  CurvyImageGeometry randomGeom, randomFullGeom;
  curvyForTarget(480, 640, {1.0, false, {}, 3.123f}, &randomGeom, &randomFullGeom);
  Vector<HPoint2> imPts;
  imPts.reserve(randomGeom.srcCols * coneGeom.srcRows);
  for (int i = 0; i < randomGeom.srcCols; i++) {
    for (int j = 0; j < randomGeom.srcRows; j++) {
      imPts.emplace_back(1.f * i, 1.f * j);
    }
  }

  Vector<HPoint3> worldPts;
  Vector<HVector3> normalPts;
  mapToGeometry(randomGeom, imPts, &worldPts, &normalPts);

  for (HPoint3 &worldPt : worldPts) {
    float r = randomGeom.radius * (worldPt.y() + 0.5) + randomGeom.radiusBottom * (0.5 - worldPt.y());
    float rSq = std::pow(r, 2);
    float xSq = std::pow(worldPt.x(), 2);
    float zSq = std::pow(worldPt.z(), 2);
    ASSERT_NEAR(xSq + zSq, rSq, 1e-6);
  }
}

TEST_F(ParameterizedGeometryTest, mapRainbowToUnconed) {
  float topRadius = 4431;
  float bottomRadius = 3179;
  CurvyImageGeometry randomGeom, randomFullGeom;
  curvyForTarget(3000, 1436, {1.0, false, {}, topRadius / bottomRadius}, &randomGeom, &randomFullGeom);
  RainbowMetadata metadata = buildRainbowMetadata(randomGeom, topRadius / randomGeom.srcRows, bottomRadius / randomGeom.srcRows);

  int outputWidth = 1600;
  int outputHeight = outputWidth * (metadata.r1 - metadata.r2) / (metadata.theta * metadata.r1);
  HPoint2 middlePt {(3000.f - 1) / 2.f, 0.f};
  HPoint2 unconedPt = mapRainbowToUnconed(metadata, 1600, 652, middlePt);
  EXPECT_NEAR(799.5, unconedPt.x(), 0.5);
  EXPECT_NEAR(0, unconedPt.y(), 0.5);

  float topLine = 2 * topRadius * std::pow(std::sin(metadata.theta / 4), 2);
  float bottomLine = topRadius - bottomRadius;

  // These tests are showing rather high (multiple full pixel difference) in mapping
  // suggesting that there might be errors in the calculation
  // TODO(dat): Reduce these errors. Check for off-by-one error
  HPoint2 unconedBottomPt = mapRainbowToUnconed(metadata, outputWidth, outputHeight, {2999.f / 2, bottomLine});
  EXPECT_NEAR((outputWidth - 1) / 2.f, unconedBottomPt.x(), 0.5);
  EXPECT_NEAR(outputHeight - 1, unconedBottomPt.y(), 3);

  HPoint2 topLeftPt = mapRainbowToUnconed(metadata, outputWidth, outputHeight, {0.f, topLine});
  EXPECT_NEAR(0, topLeftPt.x(), 0.8);
  EXPECT_NEAR(0, topLeftPt.y(), 0.5);

  HPoint2 topRight = mapRainbowToUnconed(metadata, outputWidth, outputHeight, {2999.f, topLine});
  EXPECT_NEAR(outputWidth - 1, topRight.x(), 0.5);
  EXPECT_NEAR(0, topRight.y(), 0.5);

  HPoint2 bottomLeft = mapRainbowToUnconed(metadata, outputWidth, outputHeight, {423.f, 1435.f});
  EXPECT_NEAR(0, bottomLeft.x(), 1);
  EXPECT_NEAR(outputHeight - 1, bottomLeft.y(), 1);

  HPoint2 bottomRight = mapRainbowToUnconed(metadata, outputWidth, outputHeight, {2574.f, 1435.f});
  EXPECT_NEAR(outputWidth - 1, bottomRight.x(), 3);
  EXPECT_NEAR(outputHeight - 1, bottomRight.y(), 1);
}

TEST_F(ParameterizedGeometryTest, mapRainbowToGeometry) {
  CurvyImageGeometry geom {0.33f, 1.f, {0.f, 1.f, 0.f, 1.f}, 640, 306, true, 0.24f};
  auto rainbowMetadata = buildRainbowMetadata(geom, 4463.f / 1436 , 3212.f / 1436);
  float bottomLine = (4463.f / 1436 - 3212.f / 1436) * (306 - 1);
  Vector<HPoint2> imPts = {
    {305.f, 319.5f},
    {305.f - bottomLine / 2, 319.5f},
    rotateCW(rainbowMetadata.tl, 306),
    rotateCW(rainbowMetadata.tr, 306),
    rotateCW(rainbowMetadata.bl, 306),
  };
  Vector<HPoint3> worldPts;
  mapRainbowToGeometryPoints(geom, rainbowMetadata, imPts, &worldPts);

  EXPECT_EQ(imPts.size(), worldPts.size());
  HPoint3 midPointTop {0.f, 0.5f, -geom.radius};
  EXPECT_THAT(midPointTop.data(), equalsPoint(worldPts[0]));

  EXPECT_NEAR(0, worldPts[1].x(), 1e-3);
  EXPECT_NEAR(0, worldPts[1].y(), 1e-2);
  EXPECT_NEAR(-(geom.radius + geom.radiusBottom) / 2.f, worldPts[1].z(), 1e-3);

  HPoint3 tlPoint = {0.f, 0.5f, geom.radius};
  EXPECT_THAT(tlPoint.data(), equalsPoint(worldPts[2]));

  HPoint3 trPoint = {0.f, 0.5f, geom.radius};
  EXPECT_THAT(trPoint.data(), equalsPoint(worldPts[3]));

  HPoint3 blPoint = {0.f, -0.5f, geom.radiusBottom};
  EXPECT_THAT(blPoint.data(), equalsPoint(worldPts[4], 2e-3));
}

TEST_F(ParameterizedGeometryTest, CurvyCameraRaysToVisiblePointsInCamera) {
  HMatrix nextPose = HMatrixGen::translation(-3, -5, -6);

  CurvyImageGeometry geom;
  CurvyImageGeometry fullGeom;
  curvyForTarget(480, 640, {0.6}, &geom, &fullGeom);

  Vector<HPoint3> searchPtsInModel = getPointsOnCurvy(geom);
  Vector<HPoint2> searchRaysInCamera;
  Vector<HPoint3> searchPtsInCamera;
  generateRays(
    searchPtsInModel, nextPose, &searchRaysInCamera, &searchPtsInCamera);

  Vector<HPoint3> recoveredSearchPtsInCamera;
  Vector<size_t> recoveredVisibleSearchPtIndices;
  cameraRaysToVisiblePointsInCameraCurvy(
    geom,
    searchRaysInCamera,
    nextPose,
    &recoveredSearchPtsInCamera,
    &recoveredVisibleSearchPtIndices);

  // Since the points are to the right above and in front
  // we can only see points on the left + front of the curvy
  for (int i = 3; i < 6; i++) {
    EXPECT_THAT(recoveredSearchPtsInCamera[i].data(), equalsPoint(searchPtsInCamera[i])) << format(
      "Comparing searchPtsInCamera %32s to recoveredSearchPtsInCamera %32s",
      searchPtsInCamera[i].toString().c_str(),
      recoveredSearchPtsInCamera[i].toString().c_str());
    ;
    EXPECT_TRUE(
      std::find(recoveredVisibleSearchPtIndices.begin(), recoveredVisibleSearchPtIndices.end(), i)
      != recoveredVisibleSearchPtIndices.end());
  }
  for (int i = 9; i < 12; i++) {
    EXPECT_THAT(recoveredSearchPtsInCamera[i].data(), equalsPoint(searchPtsInCamera[i])) << format(
      "Comparing searchPtsInCamera %32s to recoveredSearchPtsInCamera %32s",
      searchPtsInCamera[i].toString().c_str(),
      recoveredSearchPtsInCamera[i].toString().c_str());
    ;
    EXPECT_TRUE(
      std::find(recoveredVisibleSearchPtIndices.begin(), recoveredVisibleSearchPtIndices.end(), i)
      != recoveredVisibleSearchPtIndices.end());
  }
}

TEST_F(ParameterizedGeometryTest, PlanarCameraRaysToVisiblePointsInCamera) {
  HMatrix nextPose = HMatrixGen::i();

  Vector<HPoint2> searchPtsInSearchPixel = imageTargetCornerPixels(testK(), testK(), nextPose);
  // Adjust points bc containsPointAssumingConvex() returns false for points on the line.
  searchPtsInSearchPixel[0] = {
    searchPtsInSearchPixel[0].x() + 10.f, searchPtsInSearchPixel[0].y() + 10.f};
  searchPtsInSearchPixel[1] = {
    searchPtsInSearchPixel[1].x() + 10.f, searchPtsInSearchPixel[1].y() - 10.f};
  searchPtsInSearchPixel[2] = {
    searchPtsInSearchPixel[2].x() - 10.f, searchPtsInSearchPixel[2].y() - 10.f};
  searchPtsInSearchPixel[3] = {
    searchPtsInSearchPixel[3].x() - 10.f, searchPtsInSearchPixel[3].y() + 10.f};

  Vector<HPoint3> searchPtsInCamera =
    HMatrixGen::intrinsic(testK()).inv() * extrude<3>(searchPtsInSearchPixel);
  Vector<HPoint2> searchRaysInCamera = flatten<2>(searchPtsInCamera);

  Vector<HPoint3> recoveredSearchPtsInCamera;
  Vector<size_t> recoveredVisibleSearchPtIndices;
  cameraRaysToVisiblePointsInCameraPlanar(
    testK(),
    testK(),
    searchRaysInCamera,
    searchPtsInSearchPixel,
    nextPose,
    &recoveredSearchPtsInCamera,
    &recoveredVisibleSearchPtIndices);

  // With no difference in camera and model space our points should be the same.
  for (int i = 0; i < searchPtsInCamera.size(); ++i) {
    EXPECT_THAT(recoveredSearchPtsInCamera[i].data(), equalsPoint(searchPtsInCamera[i])) << format(
      "Comparing searchPtsInCamera %32s to recoveredSearchPtsInCamera %32s",
      searchPtsInCamera[i].toString().c_str(),
      recoveredSearchPtsInCamera[i].toString().c_str());
    ;
  }
  EXPECT_EQ(recoveredVisibleSearchPtIndices.size(), 4);
}

}  // namespace c8
