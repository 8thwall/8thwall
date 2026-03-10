// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":intrinsics",
    "//c8:hpoint",
    "//c8:hmatrix",
    "//c8/camera:device-infos",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//c8/io:capnp-messages",
    "//reality/engine/api:reality.capnp-cc",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x10f818ef);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/c8-log.h"
#include "c8/camera/device-infos.h"
#include "c8/geometry/intrinsics.h"
#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/io/capnp-messages.h"
#include "reality/engine/api/reality.capnp.h"

using testing::Eq;
using testing::FloatNear;
using testing::Pointwise;

namespace c8 {

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsMatrix(const HMatrix &matrix, float threshold = 1e-6) {
  return Pointwise(AreWithin(threshold), matrix.data());
}

decltype(auto) equalsPoint(const HPoint3 &pt, float threshold = 1e-6) {
  return Pointwise(AreWithin(threshold), pt.data());
}

decltype(auto) equalsPoint(const HPoint2 &pt, float threshold = 1e-6) {
  return Pointwise(AreWithin(threshold), pt.data());
}

namespace {
void assertIntrinsicsEqual(c8_PixelPinholeCameraModel expected, c8_PixelPinholeCameraModel actual) {
  EXPECT_EQ(expected.pixelsWidth, actual.pixelsWidth);
  EXPECT_EQ(expected.pixelsHeight, actual.pixelsHeight);
  EXPECT_EQ(expected.centerPointX, actual.centerPointX);
  EXPECT_EQ(expected.centerPointY, actual.centerPointY);
  EXPECT_EQ(expected.focalLengthHorizontal, actual.focalLengthHorizontal);
  EXPECT_EQ(expected.focalLengthVertical, actual.focalLengthVertical);
}
}  // namespace

class IntrinsicsTest : public ::testing::Test {};

TEST_F(IntrinsicsTest, TestValidGetFullFrameIntrinsics) {
  MutableRootMessage<RequestCamera> r;
  auto pixelIntrinsics = r.builder().getPixelIntrinsics();
  pixelIntrinsics.setFocalLengthHorizontal(0.0);

  MutableRootMessage<DeviceInfo> d;
  auto infoBuilder = d.builder();
  infoBuilder.setManufacturer("Google");
  infoBuilder.setModel("Pixel 3");

  auto actual = Intrinsics::getFullFrameIntrinsics(r.reader(), d.reader());
  auto expected = Intrinsics::getCameraIntrinsics(DeviceInfos::GOOGLE_PIXEL3);
  assertIntrinsicsEqual(expected, actual);
}

TEST_F(IntrinsicsTest, TestiPhoneFallbackGetFullFrameIntrinsics) {
  MutableRootMessage<RequestCamera> r;
  auto pixelIntrinsics = r.builder().getPixelIntrinsics();
  pixelIntrinsics.setFocalLengthHorizontal(0.0);

  MutableRootMessage<DeviceInfo> d;
  auto infoBuilder = d.builder();
  infoBuilder.setModel("IPHONE XSMAX/11PROMAX");

  auto actual = Intrinsics::getFullFrameIntrinsics(r.reader(), d.reader());
  auto expected = Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS_MAX);
  assertIntrinsicsEqual(expected, actual);
}

TEST_F(IntrinsicsTest, TestDefaultFallbackGetFullFrameIntrinsics) {
  MutableRootMessage<RequestCamera> r;
  auto pixelIntrinsics = r.builder().getPixelIntrinsics();
  pixelIntrinsics.setFocalLengthHorizontal(0.0);

  MutableRootMessage<DeviceInfo> d;
  auto infoBuilder = d.builder();
  infoBuilder.setModel("bogus device model");

  auto actual = Intrinsics::getFullFrameIntrinsics(r.reader(), d.reader());
  auto expected = Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_X);
  assertIntrinsicsEqual(expected, actual);
}

TEST_F(IntrinsicsTest, TestUnitTestCamera) {
  HMatrix K = Intrinsics::unitTestCamera();
  HPoint3 x(1.0f, 1.0f, 1.0f);
  auto projected = K * x;
  EXPECT_FLOAT_EQ(projected.x(), 8.0);
  EXPECT_FLOAT_EQ(projected.y(), -3.0);
}

TEST_F(IntrinsicsTest, TestGetCameraIntrinsics) {
  c8_PixelPinholeCameraModel intrinsic =
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_7);
  EXPECT_EQ(480, intrinsic.pixelsWidth);
}

TEST_F(IntrinsicsTest, TestGetCameraIntrinsicsLGEN5) {
  c8_PixelPinholeCameraModel intrinsic = Intrinsics::getCameraIntrinsics(DeviceInfos::LGE_NEXUS_5);
  EXPECT_EQ(480, intrinsic.pixelsWidth);
}

TEST_F(IntrinsicsTest, TestScaleCentered) {
  c8_PixelPinholeCameraModel intrinsic =
    c8_PixelPinholeCameraModel{480, 640, 239.5f, 319.5f, 503, 503};
  intrinsic = Intrinsics::cropAndScaleIntrinsics(intrinsic, 240, 320);
  EXPECT_EQ(320, intrinsic.pixelsHeight);
  EXPECT_EQ(240, intrinsic.pixelsWidth);
  EXPECT_EQ(251.5f, intrinsic.focalLengthHorizontal);
  EXPECT_EQ(251.5f, intrinsic.focalLengthVertical);
  EXPECT_EQ(119.5f, intrinsic.centerPointX);
  EXPECT_EQ(159.5f, intrinsic.centerPointY);
}

TEST_F(IntrinsicsTest, TestTransformCropWidth) {
  c8_PixelPinholeCameraModel intrinsic = c8_PixelPinholeCameraModel{640, 480, 320, 240, 500, 500};
  intrinsic = Intrinsics::cropAndScaleIntrinsics(intrinsic, 480, 480);
  EXPECT_EQ(480, intrinsic.pixelsHeight);
  EXPECT_EQ(480, intrinsic.pixelsWidth);
  EXPECT_EQ(500, intrinsic.focalLengthHorizontal);
  EXPECT_EQ(500, intrinsic.focalLengthVertical);
  EXPECT_EQ(240, intrinsic.centerPointX);
  EXPECT_EQ(240, intrinsic.centerPointY);
}

TEST_F(IntrinsicsTest, TestTransformCropHeight) {
  c8_PixelPinholeCameraModel intrinsic = c8_PixelPinholeCameraModel{480, 640, 240, 320, 500, 500};
  intrinsic = Intrinsics::cropAndScaleIntrinsics(intrinsic, 480, 480);
  EXPECT_EQ(480, intrinsic.pixelsHeight);
  EXPECT_EQ(480, intrinsic.pixelsWidth);
  EXPECT_EQ(500, intrinsic.focalLengthHorizontal);
  EXPECT_EQ(500, intrinsic.focalLengthVertical);
  EXPECT_EQ(240, intrinsic.centerPointX);
  EXPECT_EQ(240, intrinsic.centerPointY);
}

TEST_F(IntrinsicsTest, TestTransformCropFocalLength_Scale_1_over_2) {
  c8_PixelPinholeCameraModel intrinsic = c8_PixelPinholeCameraModel{640, 480, 320, 240, 500, 500};
  intrinsic = Intrinsics::cropAndScaleIntrinsics(intrinsic, 280, 240);
  EXPECT_EQ(240, intrinsic.pixelsHeight);
  EXPECT_EQ(280, intrinsic.pixelsWidth);
  EXPECT_EQ(250, intrinsic.focalLengthHorizontal);
  EXPECT_EQ(250, intrinsic.focalLengthVertical);
  EXPECT_EQ(139.75f, intrinsic.centerPointX);
  EXPECT_EQ(119.75f, intrinsic.centerPointY);
}

TEST_F(IntrinsicsTest, TestTransformCropCenterX) {
  c8_PixelPinholeCameraModel intrinsic = c8_PixelPinholeCameraModel{640, 480, 325, 245, 500, 500};
  intrinsic = Intrinsics::cropAndScaleIntrinsics(intrinsic, 128, 96);
  EXPECT_EQ(64.6f, intrinsic.centerPointX);
}

TEST_F(IntrinsicsTest, TestTransformCropCenterY) {
  c8_PixelPinholeCameraModel intrinsic = c8_PixelPinholeCameraModel{64, 64, 32, 48, 500, 500};
  intrinsic = Intrinsics::cropAndScaleIntrinsics(intrinsic, 16, 32);
  EXPECT_EQ(23.75f, intrinsic.centerPointY);
}

TEST_F(IntrinsicsTest, TestToClipSpace) {
  c8_PixelPinholeCameraModel intrinsic =
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS);

  // Pixels are centered at their locations, and extend half a pixel to the left and to the right.
  // If we assert that the field of view covers the whole pixel, then the real edge of the image is
  // at -0.5f and (width/height + 0.5f). Here we verify that the left and right most edge of the
  // field of view map to the extents of clip space.
  float startx = -0.5f;
  float centerx = 239.5f;
  float endx = 479.5f;
  float starty = -0.5f;
  float centery = 319.5f;
  float endy = 639.5f;

  Vector<HPoint2> pixelCorners = {
    {startx, starty},
    {endx, starty},
    {centerx, centery},
    {startx, endy},
    {endx, endy},
  };
  float kn = 0.01f;
  float kf = 1000.0f;
  auto intrinsicMat = HMatrixGen::intrinsic(intrinsic);
  auto rays = intrinsicMat.inv() * extrude<3>(pixelCorners);

  // Convert to right handed view here by making distance negative.
  Vector<HPoint3> frustumCorners = {
    {rays[0].x() * kn, rays[0].y() * kn, -kn},
    {rays[1].x() * kn, rays[1].y() * kn, -kn},
    {rays[2].x() * kn, rays[2].y() * kn, -kn},
    {rays[3].x() * kn, rays[3].y() * kn, -kn},
    {rays[4].x() * kn, rays[4].y() * kn, -kn},
    {rays[0].x() * kf, rays[0].y() * kf, -kf},
    {rays[1].x() * kf, rays[1].y() * kf, -kf},
    {rays[2].x() * kf, rays[2].y() * kf, -kf},
    {rays[3].x() * kf, rays[3].y() * kf, -kf},
    {rays[4].x() * kf, rays[4].y() * kf, -kf},
  };

  auto projection = Intrinsics::toClipSpaceMat(intrinsic, 0.01f, 1000.0f);

  auto clipCorners = projection * frustumCorners;

  // Points at the near clip plane map to -1.0f, and at the far clip plane map to 1.0f.
  // This is inconsistent with a right-handed view of clip space, but is consistent with using
  // glDepthFunc(GL_LEQUAL) for depth testing.
  EXPECT_THAT(clipCorners[0].data(), equalsPoint({-1.0f, 1.0f, -1.0f}));
  EXPECT_THAT(clipCorners[1].data(), equalsPoint({1.0f, 1.0f, -1.0f}));
  EXPECT_THAT(clipCorners[2].data(), equalsPoint({0.0f, 0.0f, -1.0f}));
  EXPECT_THAT(clipCorners[3].data(), equalsPoint({-1.0f, -1.0f, -1.0f}));
  EXPECT_THAT(clipCorners[4].data(), equalsPoint({1.0f, -1.0f, -1.0f}));
  EXPECT_THAT(clipCorners[5].data(), equalsPoint({-1.0f, 1.0f, 1.0f}));
  EXPECT_THAT(clipCorners[6].data(), equalsPoint({1.0f, 1.0f, 1.0f}));
  EXPECT_THAT(clipCorners[7].data(), equalsPoint({0.0f, 0.0f, 1.0f}));
  EXPECT_THAT(clipCorners[8].data(), equalsPoint({-1.0f, -1.0f, 1.0f}));
  EXPECT_THAT(clipCorners[9].data(), equalsPoint({1.0f, -1.0f, 1.0f}));

  EXPECT_THAT((projection.inv() * projection).data(), equalsMatrix(HMatrixGen::i(), 1e-5f));

  // Note: Since we flipped the distance sign above when switching to a right handed view, the pixel
  // locations are reversed on reconstruction.
  auto reconPixelCorners = flatten<2>((intrinsicMat * projection.inv()) * clipCorners);
  EXPECT_THAT(reconPixelCorners[0].data(), equalsPoint({endx, endy}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[1].data(), equalsPoint({startx, endy}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[2].data(), equalsPoint({centerx, centery}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[3].data(), equalsPoint({endx, starty}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[4].data(), equalsPoint({startx, starty}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[5].data(), equalsPoint({endx, endy}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[6].data(), equalsPoint({startx, endy}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[7].data(), equalsPoint({centerx, centery}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[8].data(), equalsPoint({endx, starty}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[9].data(), equalsPoint({startx, starty}, 1e-4f));
}

TEST_F(IntrinsicsTest, TestToClipBackToIntrinsics) {
  c8_PixelPinholeCameraModel intrinsic =
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS);

  auto projection = Intrinsics::toClipSpaceMat(intrinsic, 0.01f, 1000.0f);
  // Once you have the projection, we need to recover the intrinsic
  // Assuming that you know the pixelsWidth and pixelsHeight from the texture
  int32_t pixelsWidth = intrinsic.pixelsWidth;
  int32_t pixelsHeight = intrinsic.pixelsHeight;
  float focalLengthX = projection.data().data()[0] * pixelsWidth * 0.5f;
  float focalLengthY = projection.data().data()[5] * pixelsHeight * 0.5f;
  float centerX = ((1.0f - projection.data().data()[8]) * pixelsWidth - 1.f) * 0.5f;
  float centerY = ((1.0f + projection.data().data()[9]) * pixelsHeight - 1.f) * 0.5f;
  EXPECT_FLOAT_EQ(intrinsic.focalLengthHorizontal, focalLengthX);
  EXPECT_FLOAT_EQ(intrinsic.focalLengthVertical, focalLengthY);
  EXPECT_FLOAT_EQ(intrinsic.centerPointX, centerX);
  EXPECT_FLOAT_EQ(intrinsic.centerPointY, centerY);
}

TEST_F(IntrinsicsTest, TestToClipSpaceLeftHanded) {
  c8_PixelPinholeCameraModel intrinsic =
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS);

  // Pixels are centered at their locations, and extend half a pixel to the left and to the right.
  // If we assert that the field of view covers the whole pixel, then the real edge of the image is
  // at -0.5f and (width/height + 0.5f). Here we verify that the left and right most edge of the
  // field of view map to the extents of clip space.
  float startx = -0.5f;
  float centerx = 239.5f;
  float endx = 479.5f;
  float starty = -0.5f;
  float centery = 319.5f;
  float endy = 639.5f;

  Vector<HPoint2> pixelCorners = {
    {startx, starty},
    {endx, starty},
    {centerx, centery},
    {startx, endy},
    {endx, endy},
  };
  float kn = 0.01f;
  float kf = 1000.0f;
  auto intrinsicMat = HMatrixGen::intrinsic(intrinsic);
  auto rays = intrinsicMat.inv() * extrude<3>(pixelCorners);

  // Since this is left-handed, we can use positive distances.
  Vector<HPoint3> frustumCorners = {
    {rays[0].x() * kn, rays[0].y() * kn, kn},
    {rays[1].x() * kn, rays[1].y() * kn, kn},
    {rays[2].x() * kn, rays[2].y() * kn, kn},
    {rays[3].x() * kn, rays[3].y() * kn, kn},
    {rays[4].x() * kn, rays[4].y() * kn, kn},
    {rays[0].x() * kf, rays[0].y() * kf, kf},
    {rays[1].x() * kf, rays[1].y() * kf, kf},
    {rays[2].x() * kf, rays[2].y() * kf, kf},
    {rays[3].x() * kf, rays[3].y() * kf, kf},
    {rays[4].x() * kf, rays[4].y() * kf, kf},
  };

  auto projection = Intrinsics::toClipSpaceMatLeftHanded(intrinsic, 0.01f, 1000.0f);

  auto clipCorners = projection * frustumCorners;

  // Points at the near clip plane map to -1.0f, and at the far clip plane map to 1.0f.
  // This is consistent with using glDepthFunc(GL_LEQUAL) for depth testing.
  EXPECT_THAT(clipCorners[0].data(), equalsPoint({-1.0f, 1.0f, -1.0f}));
  EXPECT_THAT(clipCorners[1].data(), equalsPoint({1.0f, 1.0f, -1.0f}));
  EXPECT_THAT(clipCorners[2].data(), equalsPoint({0.0f, 0.0f, -1.0f}));
  EXPECT_THAT(clipCorners[3].data(), equalsPoint({-1.0f, -1.0f, -1.0f}));
  EXPECT_THAT(clipCorners[4].data(), equalsPoint({1.0f, -1.0f, -1.0f}));
  EXPECT_THAT(clipCorners[5].data(), equalsPoint({-1.0f, 1.0f, 1.0f}));
  EXPECT_THAT(clipCorners[6].data(), equalsPoint({1.0f, 1.0f, 1.0f}));
  EXPECT_THAT(clipCorners[7].data(), equalsPoint({0.0f, 0.0f, 1.0f}));
  EXPECT_THAT(clipCorners[8].data(), equalsPoint({-1.0f, -1.0f, 1.0f}));
  EXPECT_THAT(clipCorners[9].data(), equalsPoint({1.0f, -1.0f, 1.0f}));

  EXPECT_THAT((projection.inv() * projection).data(), equalsMatrix(HMatrixGen::i(), 1e-5));

  auto reconPixelCorners = flatten<2>((intrinsicMat * projection.inv()) * clipCorners);

  EXPECT_THAT(reconPixelCorners[0].data(), equalsPoint({startx, starty}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[1].data(), equalsPoint({endx, starty}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[2].data(), equalsPoint({centerx, centery}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[3].data(), equalsPoint({startx, endy}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[4].data(), equalsPoint({endx, endy}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[5].data(), equalsPoint({startx, starty}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[6].data(), equalsPoint({endx, starty}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[7].data(), equalsPoint({centerx, centery}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[8].data(), equalsPoint({startx, endy}, 1e-4f));
  EXPECT_THAT(reconPixelCorners[9].data(), equalsPoint({endx, endy}, 1e-4f));
}

TEST_F(IntrinsicsTest, TestPerspectiveProjectionRightHanded) {
  auto mat = Intrinsics::perspectiveProjectionRightHanded(0.5f * M_PI, 1.0f, 0.01f, 1000.0f);
  HMatrix matTest{
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, -1.00002f, -0.02f},
    {0.0f, 0.0f, -1.0f, 0.0f}};
  EXPECT_THAT(mat.data(), equalsMatrix(matTest, 1e-5));

  auto mat2 =
    Intrinsics::perspectiveProjectionRightHanded(M_PI / 3.0f, 480.0f / 640.0f, 0.01f, 1000.0f);
  HMatrix mat2Test{
    {2.3094f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.73205f, 0.0f, 0.0f},
    {0.0f, 0.0f, -1.00002f, -0.02f},
    {0.0f, 0.0f, -1.0f, 0.0f}};
  EXPECT_THAT(mat2.data(), equalsMatrix(mat2Test, 1e-5));
}

TEST_F(IntrinsicsTest, TestOrthographicProjectionRightHanded) {
  auto mat1 = Intrinsics::orthographicProjectionRightHanded(1.0f, 2.0f, 4.0f, 3.0f, 5.0f, 6.0f);
  HMatrix mat1Test{
    {2.0f, 0.0f, 0.0f, -3.0f},
    {0.0f, 2.0f, 0.0f, -7.0f},
    {0.0f, 0.0f, -2.0f, -11.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
  };

  EXPECT_THAT(mat1.data(), equalsMatrix(mat1Test, 1e-5));

  auto mat2 = mat1.inv();

  EXPECT_THAT((mat1 * mat2).data(), equalsMatrix(HMatrixGen::i(), 1e-5));

  // Right-handed, camera looks down -z
  auto mat3 = Intrinsics::orthographicProjectionRightHanded(-5.0f, 5.0f, 5.0f, -5.0f, 0.f, -5.f);
  HPoint3 points[3] = {{-5.f, 5.f, 0.f},   // Should map to -1
                       {0.f, 0.f, 2.5f},   // Center of clip space, should map to 0, 0, 0
                       {-5.1f, 0.f, 0.f}}; // Should be outside clip space
  EXPECT_THAT((mat3 * points[0]).data(), equalsPoint({-1.f, 1.f, -1.f}, 1e-5));
  EXPECT_THAT((mat3 * points[1]).data(), equalsPoint({0.f, 0.f, 0.f}, 1e-5));
  EXPECT_TRUE((mat3 * points[2]).x() < -1.f);
}

}  // namespace c8
