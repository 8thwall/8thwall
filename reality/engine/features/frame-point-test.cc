// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":frame-point",
    "//c8:parameter-data",
    "//c8/geometry:intrinsics",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x0d1520f9);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/geometry/intrinsics.h"
#include "c8/parameter-data.h"
#include "reality/engine/features/frame-point.h"

using testing::Eq;
using testing::Pointwise;

namespace c8 {

class FramePointTest : public ::testing::Test {
  void SetUp() override {
    globalParams().getOrSet("FramePoint.initialDepthCertaintyThreshold", 0.f);
  }

protected:
  FeatureSet getFeatureSet() { return {OrbFeature()}; }
};

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }
MATCHER_P(AreGreater, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) > eps; }

decltype(auto) notEqualsPoint(const HPoint2 &point) {
  return Pointwise(AreGreater(0.0001), point.data());
}

decltype(auto) equalsPoint(const HPoint2 &point) {
  return Pointwise(AreWithin(0.0001), point.data());
}

decltype(auto) equalsPoint(const HPoint3 &point) {
  return Pointwise(AreWithin(0.0001), point.data());
}

decltype(auto) equalsMatrix(const HMatrix &matrix) {
  return Pointwise(AreWithin(0.0001), matrix.data());
}

TEST_F(FramePointTest, TestProjection) {
  HPoint2 pt0 = {2.0f, 3.0f};
  HPoint2 pte = {33.0f, 62.0f};  // 61=2*13+7; 106=3*17+11

  c8_PixelPinholeCameraModel a{640, 480, 7.0f, 11.0f, 13.0f, -17.0f};
  FrameWithPoints f(a);

  f.addImagePixelPoint(pte, 0, 0, 0, getFeatureSet());

  EXPECT_THAT(f.points()[0].position().data(), equalsPoint(pt0));
  EXPECT_THAT(f.hpoints()[0].data(), equalsPoint(pt0));
  EXPECT_THAT(f.pixels()[0].data(), equalsPoint(pte));
}

TEST_F(FramePointTest, TestDepthPointCreation) {
  HMatrix extrinsic = HMatrixGen::translation(1.f, 0.f, -2.f);

  HPoint3 depthInWorld = {0.5f, -.25f, 3.f};

  auto depthInCam = (extrinsic.inv() * depthInWorld);
  auto depthInRay = depthInCam.flatten();

  c8_PixelPinholeCameraModel a{640, 480, 7.0f, 11.0f, 13.0f, -17.0f};
  FrameWithPoints f(a);
  f.addImageRayPoint(depthInRay, 0, 0, 0, getFeatureSet());
  f.setExtrinsic(extrinsic);
  f.setKnownDepth(0, depthInCam.z());
  EXPECT_THAT(f.hpoints()[0].data(), equalsPoint(depthInRay));
  EXPECT_FLOAT_EQ(f.points()[0].depth(), depthInCam.z());
  EXPECT_FLOAT_EQ(
    f.points()[0].depthWithCertainty().certainty,
    globalParams().get<float>("FramePoint.knownDepthCertainty"));

  // Now, add as a world point instead of as a ray.
  FrameWithPoints f2(a);
  f2.setExtrinsic(extrinsic);
  f2.addWorldMapPoint({0, 0}, depthInWorld, 0, 0, 0, getFeatureSet());
  EXPECT_THAT(f2.hpoints()[0].data(), equalsPoint(depthInRay));
  EXPECT_FLOAT_EQ(f.points()[0].depth(), depthInCam.z());
}

TEST_F(FramePointTest, TestClearAndClone) {
  HPoint2 pt0 = {2.0f, 3.0f};
  HPoint2 pte = {33.0f, 62.0f};  // 61=2*13+7; 106=3*17+11

  c8_PixelPinholeCameraModel a{640, 480, 7.0f, 11.0f, 13.0f, -17.0f};
  FrameWithPoints f(a);

  auto ext = HMatrixGen::translation(5.0, 5.0, 19.0);
  f.setExtrinsic(ext);

  Quaternion q(0.0f, 0.0f, 0.0f, 1.0f);
  f.setXRDevicePose(q);

  f.addImagePixelPoint(pte, 0, 0, 0, getFeatureSet());

  FrameWithPoints f2(c8_PixelPinholeCameraModel{});

  f2.clearAndClone(f);

  EXPECT_THAT(f2.points()[0].position().data(), equalsPoint(pt0));
  EXPECT_THAT(f2.hpoints()[0].data(), equalsPoint(pt0));
  EXPECT_THAT(f2.pixels()[0].data(), equalsPoint(pte));

  EXPECT_EQ(a.pixelsWidth, f2.intrinsic().pixelsWidth);
  EXPECT_EQ(a.pixelsHeight, f2.intrinsic().pixelsHeight);
  EXPECT_EQ(a.centerPointX, f2.intrinsic().centerPointX);
  EXPECT_EQ(a.centerPointY, f2.intrinsic().centerPointY);
  EXPECT_EQ(a.focalLengthHorizontal, f2.intrinsic().focalLengthHorizontal);
  EXPECT_EQ(a.focalLengthVertical, f2.intrinsic().focalLengthVertical);

  EXPECT_THAT(ext.data(), equalsMatrix(f2.extrinsic()));

  EXPECT_EQ(q.w(), f2.xrDevicePose().w());
  EXPECT_EQ(q.x(), f2.xrDevicePose().x());
  EXPECT_EQ(q.y(), f2.xrDevicePose().y());
  EXPECT_EQ(q.z(), f2.xrDevicePose().z());
}

TEST_F(FramePointTest, AddWorldMapPoint) {
  HPoint2 pte = {33.0f, 62.0f};  // 61=2*13+7; 106=3*17+11

  c8_PixelPinholeCameraModel a{640, 480, 7.0f, 11.0f, 13.0f, -17.0f};
  FrameWithPoints f(a);

  auto startingPtsSize = 10;
  for (int i = 0; i < startingPtsSize; i++) {
    f.addWorldMapPoint(WorldMapPointId{1, 2}, {0.f, 0.1f, 2.f}, 1, 9, 9, getFeatureSet());
  }
  EXPECT_EQ(startingPtsSize, f.points().size());
  EXPECT_EQ(startingPtsSize, f.store().numKeypoints());
  EXPECT_EQ(startingPtsSize, f.pixels().size());
  EXPECT_EQ(startingPtsSize, f.worldPointIndices().size());

  // Add a pixel point and not a world point.
  f.addImagePixelPoint(pte, 0, 0, 0, getFeatureSet());
  EXPECT_EQ(startingPtsSize + 1, f.points().size());
  EXPECT_EQ(startingPtsSize + 1, f.store().numKeypoints());
  EXPECT_EQ(startingPtsSize + 1, f.pixels().size());
  EXPECT_EQ(startingPtsSize, f.worldPointIndices().size());

  // Add another world point.
  f.addWorldMapPoint(WorldMapPointId{1, 2}, {0.f, 0.1f, 2.f}, 1, 9, 9, getFeatureSet());
  EXPECT_EQ(startingPtsSize + 2, f.points().size());
  EXPECT_EQ(startingPtsSize + 2, f.store().numKeypoints());
  EXPECT_EQ(startingPtsSize + 2, f.pixels().size());
  EXPECT_EQ(startingPtsSize + 1, f.worldPointIndices().size());
}

TEST_F(FramePointTest, TestClone) {
  HPoint2 pt0 = {2.0f, 3.0f};
  HPoint2 pte = {33.0f, 62.0f};  // 61=2*13+7; 106=3*17+11

  c8_PixelPinholeCameraModel a{640, 480, 7.0f, 11.0f, 13.0f, -17.0f};
  FrameWithPoints f(a);

  auto ext = HMatrixGen::translation(5.0, 5.0, 19.0);
  f.setExtrinsic(ext);

  Quaternion q(0.0f, 0.0f, 0.0f, 1.0f);
  f.setXRDevicePose(q);

  f.addImagePixelPoint(pte, 0, 0, 0, getFeatureSet());

  FrameWithPoints f2 = f.clone();

  EXPECT_THAT(f2.points()[0].position().data(), equalsPoint(pt0));
  EXPECT_THAT(f2.hpoints()[0].data(), equalsPoint(pt0));
  EXPECT_THAT(f2.pixels()[0].data(), equalsPoint(pte));

  EXPECT_EQ(a.pixelsWidth, f2.intrinsic().pixelsWidth);
  EXPECT_EQ(a.pixelsHeight, f2.intrinsic().pixelsHeight);
  EXPECT_EQ(a.centerPointX, f2.intrinsic().centerPointX);
  EXPECT_EQ(a.centerPointY, f2.intrinsic().centerPointY);
  EXPECT_EQ(a.focalLengthHorizontal, f2.intrinsic().focalLengthHorizontal);
  EXPECT_EQ(a.focalLengthVertical, f2.intrinsic().focalLengthVertical);

  EXPECT_THAT(ext.data(), equalsMatrix(f2.extrinsic()));

  EXPECT_EQ(q.w(), f2.xrDevicePose().w());
  EXPECT_EQ(q.x(), f2.xrDevicePose().x());
  EXPECT_EQ(q.y(), f2.xrDevicePose().y());
  EXPECT_EQ(q.z(), f2.xrDevicePose().z());
}

TEST_F(FramePointTest, TestCloneForNewMap) {
  HPoint2 pt0 = {2.0f, 3.0f};
  HPoint2 pte = {33.0f, 62.0f};  // 61=2*13+7; 106=3*17+11

  c8_PixelPinholeCameraModel a{640, 480, 7.0f, 11.0f, 13.0f, -17.0f};
  FrameWithPoints f(a);

  auto ext = HMatrixGen::translation(5.0, 5.0, 19.0);
  f.setExtrinsic(ext);

  Quaternion q(0.0f, 0.0f, 0.0f, 1.0f);
  f.setXRDevicePose(q);

  f.addImagePixelPoint(pte, 0, 0, 0, getFeatureSet());

  f.setWorldPoint(0, WorldMapPointId{0, 1});

  EXPECT_EQ(f.worldPointIndices().size(), 1);

  f.addWorldMapPoint(WorldMapPointId{1, 2}, {0.f, 0.1f, 2.f}, 1, 9, 9, getFeatureSet());

  FrameWithPoints f2 = f.cloneForNewMap();

  // The cloned frame points should not be associated with any world points or keyframes.
  EXPECT_TRUE(f2.worldPointIndices().empty());

  // Each point should not be associated with a map point
  EXPECT_EQ(f2.points()[0].id(), WorldMapPointId::nullId());
  EXPECT_EQ(f2.points()[1].id(), WorldMapPointId::nullId());

  EXPECT_THAT(f2.points()[0].position().data(), equalsPoint(pt0));
  EXPECT_THAT(f2.hpoints()[0].data(), equalsPoint(pt0));
  EXPECT_THAT(f2.pixels()[0].data(), equalsPoint(pte));

  EXPECT_THAT(f2.points()[0].position().data(), equalsPoint(pt0));
  EXPECT_THAT(f2.hpoints()[0].data(), equalsPoint(pt0));
  EXPECT_THAT(f2.pixels()[0].data(), equalsPoint(pte));

  EXPECT_EQ(a.pixelsWidth, f2.intrinsic().pixelsWidth);
  EXPECT_EQ(a.pixelsHeight, f2.intrinsic().pixelsHeight);
  EXPECT_EQ(a.centerPointX, f2.intrinsic().centerPointX);
  EXPECT_EQ(a.centerPointY, f2.intrinsic().centerPointY);
  EXPECT_EQ(a.focalLengthHorizontal, f2.intrinsic().focalLengthHorizontal);
  EXPECT_EQ(a.focalLengthVertical, f2.intrinsic().focalLengthVertical);

  EXPECT_THAT(ext.data(), equalsMatrix(f2.extrinsic()));

  EXPECT_EQ(q.w(), f2.xrDevicePose().w());
  EXPECT_EQ(q.x(), f2.xrDevicePose().x());
  EXPECT_EQ(q.y(), f2.xrDevicePose().y());
  EXPECT_EQ(q.z(), f2.xrDevicePose().z());
}

TEST_F(FramePointTest, TestWorldPosition) {
  HPoint2 pt0 = {2.0f, 3.0f};
  HPoint2 pte = {33.0f, 62.0f};  // 61=2*13+7; 106=3*17+11

  HPoint3 wpt = {0.0f, 0.0f, 3.0f};
  HMatrix pos = HMatrixGen::translation(-4.0f, -6.0f, 1.0f);
  WorldMapPointId id(5, 8);

  c8_PixelPinholeCameraModel a{640, 480, 7.0f, 11.0f, 13.0f, -17.0f};
  FrameWithPoints f(a, pos);

  f.addWorldMapPoint(id, wpt, 3, 0, 0, getFeatureSet());

  FrameWithPoints f2(c8_PixelPinholeCameraModel{});

  f2.clearAndClone(f);

  EXPECT_EQ(2.0f, f2.points()[0].depth());
  EXPECT_EQ(id, f2.points()[0].id());
  EXPECT_THAT(f2.points()[0].position().data(), equalsPoint(pt0));
  EXPECT_THAT(f2.hpoints()[0].data(), equalsPoint(pt0));
  EXPECT_THAT(f2.pixels()[0].data(), equalsPoint(pte));
  EXPECT_THAT(f2.worldPoints()[0].data(), equalsPoint(wpt));
}

TEST_F(FramePointTest, TestAddRemoveViews) {
  Vector<HPoint3> mapPoints{
    {0.0f, 0.0f, 3.0f},
    {1.0f, 2.0f, 4.0f},
    {-.1f, -.1f, 2.0f},
  };

  Vector<WorldMapPointId> pointIds = {
    {0, 1},
    {1, 2},
    {2, 3},
  };

  // Three camera views.
  auto cam0 = HMatrixGen::i();
  auto cam1 = HMatrixGen::translation(0, 0, -0.1f);       // pt 0 is on FOE.
  auto cam2 = HMatrixGen::translation(0.1f, 0.2f, 0.4f);  // pt 1 is on FOE.

  // Image points in three cameras.
  auto rays0 = flatten<2>(cam0.inv() * mapPoints);
  auto rays1 = flatten<2>(cam1.inv() * mapPoints);
  auto rays2 = flatten<2>(cam2.inv() * mapPoints);

  // Mix up point order a little bit. rays0 has A, B, C
  std::swap(rays1[1], rays1[2]);  // rays1 has A, C, B
  std::swap(rays2[0], rays2[2]);  // rays2 has C, B, A

  // Construct three frames.
  c8_PixelPinholeCameraModel a =
    Intrinsics::getProcessingIntrinsics(DeviceInfos::DeviceModel::APPLE_IPHONE_X, 480, 640);
  FrameWithPoints f0(a, cam0);
  FrameWithPoints f1(a, cam1);
  FrameWithPoints f2(a, cam2);

  Vector<WorldMapKeyframeId> kfIds = {
    {0, 4},
    {1, 5},
    {2, 6},
  };

  // Add points to frames.
  f0.addImageRayPoint(rays0[0], 0, 0, 0, {});
  f0.addImageRayPoint(rays0[1], 0, 0, 0, {});
  f0.addImageRayPoint(rays0[2], 0, 0, 0, {});
  f0.setWorldPoint(0, pointIds[0]);
  f0.setWorldPoint(1, pointIds[1]);
  f0.setWorldPoint(2, pointIds[2]);

  f1.addImageRayPoint(rays1[0], 0, 0, 0, {});
  f1.addImageRayPoint(rays1[1], 0, 0, 0, {});
  f1.addImageRayPoint(rays1[2], 0, 0, 0, {});
  f1.setWorldPoint(0, pointIds[0]);
  f1.setWorldPoint(1, pointIds[2]);
  f1.setWorldPoint(2, pointIds[1]);

  f2.addImageRayPoint(rays2[0], 0, 0, 0, {});
  f2.addImageRayPoint(rays2[1], 0, 0, 0, {});
  f2.addImageRayPoint(rays2[2], 0, 0, 0, {});
  f2.setWorldPoint(0, pointIds[2]);
  f2.setWorldPoint(1, pointIds[1]);
  f2.setWorldPoint(2, pointIds[0]);

  // Construct associated point views.
  f0.addPointView(kfIds[1], f1, 0, 0);
  f0.addPointView(kfIds[1], f1, 2, 1);
  f0.addPointView(kfIds[1], f1, 1, 2);
  f0.addPointView(kfIds[2], f2, 2, 0);
  f0.addPointView(kfIds[2], f2, 1, 1);
  f0.addPointView(kfIds[2], f2, 0, 2);

  // Check expected triangulations.
  EXPECT_NEAR(f0.points()[0].depthWithCertainty().depthInWords, 3.0f, 4e-6f);
  EXPECT_NEAR(f0.points()[1].depthWithCertainty().depthInWords, 4.0f, 4e-6f);
  EXPECT_NEAR(f0.points()[2].depthWithCertainty().depthInWords, 2.0f, 4e-6f);

  // All triangulations are currently valid.
  EXPECT_GT(f0.points()[0].depthWithCertainty().certainty, 0.0f);
  EXPECT_GT(f0.points()[1].depthWithCertainty().certainty, 0.0f);
  EXPECT_GT(f0.points()[2].depthWithCertainty().certainty, 0.0f);

  // Each point has two views.
  EXPECT_EQ(f0.points()[0].keyframeViews().size(), 2);
  EXPECT_EQ(f0.points()[1].keyframeViews().size(), 2);
  EXPECT_EQ(f0.points()[2].keyframeViews().size(), 2);

  // Remove one view of point 2, since both views are valid, the depth remains valid.
  f0.removePointView(kfIds[2], 2);
  EXPECT_NEAR(f0.points()[2].depthWithCertainty().depthInWords, 2.0f, 4e-6f);
  EXPECT_EQ(f0.points()[2].keyframeViews().size(), 1);

  // Remove the other view from point 2, since both views are removed, the depth is invalid.
  f0.removePointView(kfIds[1], 2);
  EXPECT_FLOAT_EQ(f0.points()[2].depthWithCertainty().certainty, 0.0f);
  EXPECT_EQ(f0.points()[2].keyframeViews().size(), 0);

  // Remove the only valid view from point 0. Since no valid view remains, the depth is invalid.
  f0.removePointView(kfIds[2], 0);
  EXPECT_FLOAT_EQ(f0.points()[0].depthWithCertainty().certainty, 0.0f);
  EXPECT_EQ(f0.points()[0].keyframeViews().size(), 1);

  // Disassociate point 1 from the map. The depth should no longer be valid.
  f0.setWorldPoint(1, WorldMapPointId::nullId());
  EXPECT_FLOAT_EQ(f0.points()[1].depthWithCertainty().certainty, 0.0f);
  EXPECT_EQ(f0.points()[1].keyframeViews().size(), 0);
}

TEST_F(FramePointTest, TestUpdateIntrinsics) {
  auto cam = HMatrixGen::translation(0.1f, 0.0f, 0.0f);
  auto ray = (cam.inv() * HPoint3(0.0f, 0.0f, 3.0f)).flatten();
  HPoint2 pt = {33.0f, 62.0f};

  c8_PixelPinholeCameraModel intrinsics0 =
    Intrinsics::getProcessingIntrinsics(DeviceInfos::APPLE_IPHONE_6, 480, 640);
  FrameWithPoints f0(intrinsics0);
  f0.addImagePixelPoint(pt, 0, 0, 0, getFeatureSet());
  f0.addImageRayPoint(ray, 0, 0, 0, {});

  // Create a frame with different intrinsics.
  c8_PixelPinholeCameraModel intrinsics1 =
    Intrinsics::getProcessingIntrinsics(DeviceInfos::DeviceModel::APPLE_IPHONE_X, 480, 640);
  FrameWithPoints f1(intrinsics1);
  f1.addImagePixelPoint(pt, 0, 0, 0, getFeatureSet());
  f1.addImageRayPoint(ray, 0, 0, 0, {});

  EXPECT_THAT(f0.points()[0].position().data(), notEqualsPoint(f1.points()[0].position()));
  EXPECT_THAT(f0.hpoints()[0].data(), notEqualsPoint(f1.hpoints()[0]));
  EXPECT_THAT(f0.pixels()[0].data(), equalsPoint(f1.pixels()[0].data()));

  // Expect that when we update the intrinsics of the first frame-with-points these should be equal.
  f0.updateIntrinsics(intrinsics1);
  EXPECT_THAT(f0.points()[0].position().data(), equalsPoint(f1.points()[0].position()));
  EXPECT_THAT(f0.hpoints()[0].data(), equalsPoint(f1.hpoints()[0]));
  EXPECT_THAT(f0.pixels()[0].data(), equalsPoint(f1.pixels()[0]));
}

TEST_F(FramePointTest, TestUpdateViews) {
  Vector<HPoint3> mapPoints{
    {0.0f, 0.0f, 3.0f},
    {1.0f, 2.0f, 4.0f},
    {-.1f, -.1f, 2.0f},
  };

  Vector<WorldMapPointId> pointIds = {
    {0, 1},
    {1, 2},
    {2, 3},
  };

  // Three camera views. We will triangulate the same two images with cam0->cam1, cam0->cam2 and
  // cam1->cam2. cam0->cam1 and cam1->cam2 should give the same depth results, while cam0->cam2
  // should give depth values that are twice as far away.
  auto cam0 = HMatrixGen::translation(0.1f, 0.0f, 0.0f);
  auto cam1 = HMatrixGen::i();                             // Move left by 0.1
  auto cam2 = HMatrixGen::translation(-0.1f, 0.0f, 0.0f);  // Move left by 0.1 again.

  // Image points in two cameras.
  auto rays0 = flatten<2>(cam0.inv() * mapPoints);
  auto rays1 = flatten<2>(cam1.inv() * mapPoints);

  // Mix up point order a little bit. rays0 has A, B, C
  std::swap(rays1[1], rays1[2]);  // rays1 has A, C, B

  // Construct three frames.
  c8_PixelPinholeCameraModel a =
    Intrinsics::getProcessingIntrinsics(DeviceInfos::DeviceModel::APPLE_IPHONE_X, 480, 640);
  FrameWithPoints f0(a, cam0);
  FrameWithPoints f1(a, cam1);

  Vector<WorldMapKeyframeId> kfIds = {
    {0, 4},
    {1, 5},
  };

  // Add points to frames.
  f0.addImageRayPoint(rays0[0], 0, 0, 0, {});
  f0.addImageRayPoint(rays0[1], 0, 0, 0, {});
  f0.addImageRayPoint(rays0[2], 0, 0, 0, {});
  f0.setWorldPoint(0, pointIds[0]);
  f0.setWorldPoint(1, pointIds[1]);
  f0.setWorldPoint(2, pointIds[2]);

  f1.addImageRayPoint(rays1[0], 0, 0, 0, {});
  f1.addImageRayPoint(rays1[1], 0, 0, 0, {});
  f1.addImageRayPoint(rays1[2], 0, 0, 0, {});
  f1.setWorldPoint(0, pointIds[0]);
  f1.setWorldPoint(1, pointIds[2]);
  f1.setWorldPoint(2, pointIds[1]);

  // Construct associated point views.
  f0.addPointView(kfIds[1], f1, 0, 0);
  f0.addPointView(kfIds[1], f1, 2, 1);
  f0.addPointView(kfIds[1], f1, 1, 2);

  // Check expected triangulations.
  EXPECT_NEAR(f0.points()[0].depthWithCertainty().depthInWords, 3.0f, 4e-6f);
  EXPECT_NEAR(f0.points()[1].depthWithCertainty().depthInWords, 4.0f, 4e-6f);
  EXPECT_NEAR(f0.points()[2].depthWithCertainty().depthInWords, 2.0f, 4e-6f);

  // Now move the second camera so that its translation is twice as big, but the points move the
  // same amount. This means they're twice as far away.
  f1.setExtrinsic(cam2);
  f0.updateCurrentPointViews(kfIds[1], f1);

  // Check expected triangulations. They're twice the distance.
  EXPECT_NEAR(f0.points()[0].depthWithCertainty().depthInWords, 6.0f, 4e-6f);
  EXPECT_NEAR(f0.points()[1].depthWithCertainty().depthInWords, 8.0f, 4e-6f);
  EXPECT_NEAR(f0.points()[2].depthWithCertainty().depthInWords, 4.0f, 4e-6f);

  // Now move the first camera so that the translation is the same as the original (but with
  // different positions). The points should be at their original depths.
  f0.setExtrinsic(cam1);
  f0.updateCurrentPointViews(kfIds[1], f1);

  // Check expected triangulations. They're at the original depth.
  EXPECT_NEAR(f0.points()[0].depthWithCertainty().depthInWords, 3.0f, 4e-6f);
  EXPECT_NEAR(f0.points()[1].depthWithCertainty().depthInWords, 4.0f, 4e-6f);
  EXPECT_NEAR(f0.points()[2].depthWithCertainty().depthInWords, 2.0f, 4e-6f);

  // If pass in "skipPrework" = true then we should get the same result as above even though we have
  // changed the extrinsic.
  f0.setExtrinsic(cam2);
  f0.updateCurrentPointViews(kfIds[1], f1, true);

  // Check expected triangulations. They're at the original depth.
  EXPECT_NEAR(f0.points()[0].depthWithCertainty().depthInWords, 3.0f, 4e-6f);
  EXPECT_NEAR(f0.points()[1].depthWithCertainty().depthInWords, 4.0f, 4e-6f);
  EXPECT_NEAR(f0.points()[2].depthWithCertainty().depthInWords, 2.0f, 4e-6f);
}

TEST_F(FramePointTest, BestPointMatchesTest) {
  BestPointMatches matches(4);

  matches.push({0, 0, -1.f, 1.f, SCALE_UNKNOWN});
  matches.push({1, 1, -1.f, 3.f, SCALE_UNKNOWN});
  matches.push({2, 2, -1.f, 2.f, SCALE_UNKNOWN});
  matches.push({3, 3, -1.f, 4.f, SCALE_UNKNOWN});
  matches.push({4, 4, -1.f, 0.f, SCALE_UNKNOWN});
  matches.push({5, 5, -1.f, 5.f, SCALE_UNKNOWN});

  auto sortedMatches = matches.sortedMatches();
  EXPECT_EQ(sortedMatches.size(), 4);

  EXPECT_EQ(sortedMatches[0].dictionaryIdx, 4);
  EXPECT_EQ(sortedMatches[0].descriptorDist, 0.f);

  EXPECT_EQ(sortedMatches[1].dictionaryIdx, 0);
  EXPECT_EQ(sortedMatches[1].descriptorDist, 1.f);

  EXPECT_EQ(sortedMatches[2].dictionaryIdx, 2);
  EXPECT_EQ(sortedMatches[2].descriptorDist, 2.f);

  EXPECT_EQ(sortedMatches[3].dictionaryIdx, 1);
  EXPECT_EQ(sortedMatches[3].descriptorDist, 3.f);
}

}  // namespace c8
