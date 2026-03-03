// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":detection-image",
    "//c8/geometry:intrinsics",
    "//c8/io:capnp-messages",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x8c208439);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/c8-log.h"
#include "c8/geometry/intrinsics.h"
#include "c8/io/capnp-messages.h"
#include "reality/engine/imagedetection/detection-image.h"

using testing::Eq;
using testing::FloatNear;
using testing::Pointwise;

namespace c8 {

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

static c8_PixelPinholeCameraModel testK() {
  return Intrinsics::getCameraIntrinsics(DeviceInfos::GOOGLE_PIXEL3);
}

class DetectionImageTest : public ::testing::Test {};

TEST_F(DetectionImageTest, toCurvyGeometryMessage) {
  TargetWithPoints framePoints(testK());

  CurvyImageGeometry geom;
  CurvyImageGeometry fullGeom;
  curvyForTarget(306, 640, {1.f, false, {2.f, 2.f, 0.25f, 0.25f}, 1.375}, &geom, &fullGeom);
  DetectionImage detectionImage(
    "curvy", std::move(framePoints), 0, geom, fullGeom, HMatrixGen::i());

  MutableRootMessage<CurvyGeometry> curvyGeometryMsg;
  auto curvyGeometry = curvyGeometryMsg.builder();
  detectionImage.toCurvyGeometry(curvyGeometry);
  CurvyGeometry::Reader reader = curvyGeometryMsg.reader();

  EXPECT_NEAR(reader.getCurvyCircumferenceTop(), fullGeom.radius * 2 * M_PI, 1e-6);
  EXPECT_NEAR(reader.getCurvyCircumferenceBottom(), fullGeom.radiusBottom * 2 * M_PI, 1e-6);
  float sideLength =
    std::sqrt(std::pow(fullGeom.radius - fullGeom.radiusBottom, 2) + std::pow(fullGeom.height, 2));
  EXPECT_NEAR(reader.getCurvySideLength(), sideLength, 1e-6);

  EXPECT_NEAR(reader.getHeight(), fullGeom.height, 1e-6);
  EXPECT_NEAR(reader.getTopRadius(), fullGeom.radius, 1e-6);
  EXPECT_NEAR(reader.getBottomRadius(), fullGeom.radiusBottom, 1e-6);
  EXPECT_NEAR(
    reader.getArcLengthRadians(),
    (fullGeom.activationRegion.right - fullGeom.activationRegion.left) * 2 * M_PI,
    1e-6);
  EXPECT_NEAR(reader.getArcStartRadians(), fullGeom.activationRegion.left * 2 * M_PI, 1e-6);
}

}  // namespace c8
