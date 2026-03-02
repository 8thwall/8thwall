// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":face-geometry",
    "//c8/camera:device-infos",
    "//c8/geometry:facemesh-data",
    "//c8/geometry:intrinsics",
    "//c8/geometry:mesh",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x530705a6);

#include <gmock/gmock.h>

#include "c8/camera/device-infos.h"
#include "c8/geometry/facemesh-data.h"
#include "c8/geometry/intrinsics.h"
#include "c8/geometry/mesh.h"
#include "c8/stats/scope-timer.h"
#include "gtest/gtest.h"
#include "reality/engine/faces/face-geometry.h"

namespace c8 {

class FaceGeometryTest : public ::testing::Test {};

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsMatrix(const HMatrix &matrix) {
  return testing::Pointwise(AreWithin(0.0001), matrix.data());
}

decltype(auto) equalsPoint(const HPoint2 &point) {
  return testing::Pointwise(AreWithin(0.0001), point.data());
}

decltype(auto) equalsPoint(const HPoint3 &point) {
  return testing::Pointwise(AreWithin(0.0001), point.data());
}

TEST_F(FaceGeometryTest, TestIntersectionOverUnion) {
  BoundingBox a = {
    {0.f, 1.f},  // upper left
    {1.f, 1.f},  // upper right
    {0.f, 0.f},  // lower left
    {1.f, 0.f},  // lower right
  };
  // Slid over to the left 3 units.
  BoundingBox b = {
    {-3.f, 1.f},  // upper left
    {-2.f, 1.f},  // upper right
    {-3.f, 0.f},  // lower left
    {-2.f, 0.f},  // lower right
  };
  EXPECT_FLOAT_EQ(0.f, intersectionOverUnion(a, b));
  EXPECT_FLOAT_EQ(1.f, intersectionOverUnion(a, a));

  // Overlapping
  BoundingBox c = {
    {0.5f, 1.5f},  // upper left
    {1.5f, 1.5f},  // upper right
    {0.5f, 0.5f},  // lower left
    {1.5f, 0.5f},  // lower right
  };
  EXPECT_FLOAT_EQ(0.1428571f, intersectionOverUnion(a, c));
}

TEST_F(FaceGeometryTest, uprightBBox) {
  BoundingBox a = {
    {0.f, 1.f},  // upper left
    {1.f, 1.f},  // upper right
    {0.f, 0.f},  // lower left
    {1.f, 0.f},  // lower right
  };
  auto uprightA = uprightBBox(a);
  EXPECT_FLOAT_EQ(uprightA.upperLeft.x(), 0.f);
  EXPECT_FLOAT_EQ(uprightA.upperLeft.y(), 1.f);
  EXPECT_FLOAT_EQ(uprightA.upperRight.x(), 1.f);
  EXPECT_FLOAT_EQ(uprightA.upperRight.y(), 1.f);
  EXPECT_FLOAT_EQ(uprightA.lowerLeft.x(), 0.f);
  EXPECT_FLOAT_EQ(uprightA.lowerLeft.y(), 0.f);
  EXPECT_FLOAT_EQ(uprightA.lowerRight.x(), 1.f);
  EXPECT_FLOAT_EQ(uprightA.lowerRight.y(), 0.f);

  // Box rotated 45 degrees clockwise.
  BoundingBox b = {
    {0.5f, 1.f},  // upper left
    {1.f, 0.5f},  // upper right
    {0.f, 0.5f},  // lower left
    {0.5f, 0.f},  // lower right
  };
  auto uprightB = uprightBBox(b);
  EXPECT_FLOAT_EQ(uprightB.upperLeft.x(), 0.f);
  EXPECT_FLOAT_EQ(uprightB.upperLeft.y(), 1.f);
  EXPECT_FLOAT_EQ(uprightB.upperRight.x(), 1.f);
  EXPECT_FLOAT_EQ(uprightB.upperRight.y(), 1.f);
  EXPECT_FLOAT_EQ(uprightB.lowerLeft.x(), 0.f);
  EXPECT_FLOAT_EQ(uprightB.lowerLeft.y(), 0.f);
  EXPECT_FLOAT_EQ(uprightB.lowerRight.x(), 1.f);
  EXPECT_FLOAT_EQ(uprightB.lowerRight.y(), 0.f);
}

TEST_F(FaceGeometryTest, TestMinBBoxDistance) {
  BoundingBox a = {
    {0.f, 1.f},  // upper left
    {1.f, 1.f},  // upper right
    {0.f, 0.f},  // lower left
    {1.f, 0.f},  // lower right
  };
  // Slid over to the left 3 units.
  BoundingBox b = {
    {-3.f, 1.f},  // upper left
    {-2.f, 1.f},  // upper right
    {-3.f, 0.f},  // lower left
    {-2.f, 0.f},  // lower right
  };
  EXPECT_FLOAT_EQ(2.f, minBBoxDistance(a, b));
  EXPECT_FLOAT_EQ(0.f, minBBoxDistance(a, a));
  // Down and left 3 units
  BoundingBox c = {
    {-3.f, -2.f},  // upper left
    {-2.f, -2.f},  // upper right
    {-3.f, -3.f},  // lower left
    {-2.f, -3.f},  // lower right
  };
  EXPECT_FLOAT_EQ(2.8284271f, minBBoxDistance(a, c));

  // Overlapping should equal 0.
  BoundingBox d = {
    {0.5f, 1.5f},  // upper left
    {1.5f, 1.5f},  // upper right
    {0.5f, 0.5f},  // lower left
    {1.5f, 0.5f},  // lower right
  };
  EXPECT_FLOAT_EQ(0.f, minBBoxDistance(a, d));

  // Right on the edge should equal 0.
  BoundingBox e = {
    {1.f, 1.f},  // upper left
    {2.f, 1.f},  // upper right
    {1.f, 0.f},  // lower left
    {2.f, 0.f},  // lower right
  };
  EXPECT_FLOAT_EQ(0.f, minBBoxDistance(a, e));
}

TEST_F(FaceGeometryTest, TestDetectionToImageSpace) {
  DetectedPoints ptsWithBb;
  ptsWithBb.intrinsics = Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6);
  ptsWithBb.boundingBox = {
    {0.2f, 0.3f},  // upper left
    {0.3f, 0.3f},  // upper right
    {0.2f, 0.5f},  // lower left
    {0.3f, 0.5f},  // lower right
  };
  auto transform = detectionRoiNoPadding(ptsWithBb);

  DetectedPoints a{
    1.0f,                                              // confidence
    0,                                                 // detectedClass
    {0, 0, 128, 128},                                  // viewport
    {ImageRoi::Source::FACE, 0, "", HMatrixGen::i()},  // roi
    {
      // boundingBox
      {.25f, 0.5f},  // upper left
      {.75f, 0.5f},  // upper right
      {.25f, 1.0f},  // lower left
      {.75f, 1.0f},  // lower right
    },
    {{0.5f, 0.75f, 0.0f}, {0.75f, 0.5f, -1.0f}},                   // points
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6),  // intrinsics
  };
  DetectedPoints b{
    0.8f,                                                          // confidence
    0,                                                             // detectedClass
    {32, 64, 128, 256},                                            // viewport
    transform,                                                     // roi
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}},      // boundingBox
    {{0.5f, 0.25f, 1.0f}, {0.75f, 1.0f, 0.0f}},                    // points
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6),  // intrinsics
  };
  Vector<DetectedPoints> d{a, b};

  auto ia = detectionToImageSpace(a);
  auto ib = detectionToImageSpace(b);

  // Vector version
  auto id = detectionToImageSpace(d);

  // Bounding box
  BoundingBox bImageBbox = detectionBoundingBoxToImageSpace(b.boundingBox, b.roi);

  Vector<DetectedImagePoints> as = {ia, id[0]};
  for (const auto &p : as) {
    EXPECT_FLOAT_EQ(1, p.confidence);
    EXPECT_FLOAT_EQ(0, p.viewport.x);
    EXPECT_FLOAT_EQ(0, p.viewport.y);
    EXPECT_FLOAT_EQ(128, p.viewport.w);
    EXPECT_FLOAT_EQ(128, p.viewport.h);
    EXPECT_THAT(p.roi.warp.data(), equalsMatrix(HMatrixGen::i()));
    EXPECT_FLOAT_EQ(.25f, p.boundingBox.upperLeft.x());
    EXPECT_FLOAT_EQ(.5f, p.boundingBox.upperLeft.y());
    EXPECT_FLOAT_EQ(.5f, p.boundingBox.width());
    EXPECT_FLOAT_EQ(.5f, p.boundingBox.height());
    EXPECT_FLOAT_EQ(0.5f, p.points[0].x());
    EXPECT_FLOAT_EQ(0.75f, p.points[0].y());
    EXPECT_FLOAT_EQ(0, p.points[0].z());
    EXPECT_FLOAT_EQ(0.75f, p.points[1].x());
    EXPECT_FLOAT_EQ(0.5f, p.points[1].y());
    EXPECT_FLOAT_EQ(-1, p.points[1].z());
  }

  Vector<DetectedImagePoints> bs = {ib, id[1]};
  for (const auto &p : bs) {
    EXPECT_FLOAT_EQ(0.8f, p.confidence);
    EXPECT_FLOAT_EQ(32, p.viewport.x);
    EXPECT_FLOAT_EQ(64, p.viewport.y);
    EXPECT_FLOAT_EQ(128, p.viewport.w);
    EXPECT_FLOAT_EQ(256, p.viewport.h);
    EXPECT_THAT(p.roi.warp.data(), equalsMatrix(transform.warp));
    EXPECT_FLOAT_EQ(0.2f, p.boundingBox.upperLeft.x());
    EXPECT_FLOAT_EQ(0.3f, p.boundingBox.upperLeft.y());
    EXPECT_FLOAT_EQ(0.1f, p.boundingBox.width());
    EXPECT_FLOAT_EQ(0.2f, p.boundingBox.height());
    EXPECT_FLOAT_EQ(0.25f, p.points[0].x());
    EXPECT_FLOAT_EQ(0.35f, p.points[0].y());
    EXPECT_FLOAT_EQ(1, p.points[0].z());
    EXPECT_FLOAT_EQ(0.275f, p.points[1].x());
    EXPECT_FLOAT_EQ(0.5f, p.points[1].y());
    EXPECT_FLOAT_EQ(0, p.points[1].z());
  }

  // compare bounding box
  EXPECT_FLOAT_EQ(ib.boundingBox.upperLeft.x(), bImageBbox.upperLeft.x());
  EXPECT_FLOAT_EQ(ib.boundingBox.upperRight.x(), bImageBbox.upperRight.x());
  EXPECT_FLOAT_EQ(ib.boundingBox.lowerLeft.x(), bImageBbox.lowerLeft.x());
  EXPECT_FLOAT_EQ(ib.boundingBox.lowerRight.x(), bImageBbox.lowerRight.x());
}

TEST_F(FaceGeometryTest, TestComputeAnchorTransform) {
  DetectedPoints a{
    1.0f,                                              // confidence
    0,                                                 // detectedClass
    {0, 0, 128, 128},                                  // viewport
    {ImageRoi::Source::FACE, 0, "", HMatrixGen::i()},  // roi
    {
      // boundingBox
      {.25f, 0.5f},  // upper left
      {.75f, 0.5f},  // upper right
      {.25f, 1.0f},  // lower left
      {.75f, 1.0f},  // lower right
    },
    {{0.5f, 0.75f, 0.0f}, {0.75f, 0.5f, -1.0f}},                   // points
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6),  // intrinsics
  };

  auto rayDetections = detectionToRaySpace(a);
  auto faceAnchor = computeAnchorTransform(rayDetections);
  EXPECT_FLOAT_EQ(faceAnchor.position.x(), 0.0f * 0.32625365f);
  EXPECT_FLOAT_EQ(faceAnchor.position.y(), -0.28418082f * 0.32625365f);
  EXPECT_FLOAT_EQ(faceAnchor.position.z(), 0.32625365f);
  EXPECT_FLOAT_EQ(faceAnchor.rotation.w(), 0.0f);
  EXPECT_FLOAT_EQ(faceAnchor.rotation.x(), 0.0f);
  EXPECT_FLOAT_EQ(faceAnchor.rotation.y(), 1.0f);
  EXPECT_FLOAT_EQ(faceAnchor.rotation.z(), 0.0f);
  EXPECT_FLOAT_EQ(faceAnchor.scale, FACE_WIDTH);
  EXPECT_FLOAT_EQ(faceAnchor.scaledWidth, 1.0f);

  DetectedPoints ptsWithBb;
  ptsWithBb.boundingBox = {
    {0.2f, 0.3f},  // upper left
    {0.3f, 0.3f},  // upper right
    {0.2f, 0.5f},  // lower left
    {0.3f, 0.5f},  // lower right
  };
  ptsWithBb.intrinsics = Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6);
  auto transform = detectionRoiNoPadding(ptsWithBb);

  DetectedPoints b{
    0.8f,                                                          // confidence
    0,                                                             // detectedClass
    {32, 64, 128, 256},                                            // viewport
    transform,                                                     // roi
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}},      // boundingBox
    {{0.5f, 0.25f, 1.0f}, {0.75f, 1.0f, 0.0f}},                    // points
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6),  // intrinsics
  };

  rayDetections = detectionToRaySpace(b);
  faceAnchor = computeAnchorTransform(rayDetections);
  EXPECT_FLOAT_EQ(faceAnchor.position.x(), -0.21302444f * 1.6312683f);
  EXPECT_FLOAT_EQ(faceAnchor.position.y(), 0.11367232f * 1.6312683f);
  EXPECT_FLOAT_EQ(faceAnchor.position.z(), 1.6312683f);
  EXPECT_FLOAT_EQ(faceAnchor.rotation.w(), 0.0f);
  EXPECT_FLOAT_EQ(faceAnchor.rotation.x(), 0.0f);
  EXPECT_FLOAT_EQ(faceAnchor.rotation.y(), 1.0f);
  EXPECT_FLOAT_EQ(faceAnchor.rotation.z(), 0.0f);
  EXPECT_FLOAT_EQ(faceAnchor.scale, FACE_WIDTH);
  EXPECT_FLOAT_EQ(faceAnchor.scaledWidth, 1.0f);
}

TEST_F(FaceGeometryTest, TestDetectionRoi) {
  DetectedPoints ptsWithBb;
  // this is the interesting data here.
  ptsWithBb.boundingBox = {
    {0.2f, 0.3f},  // upper left
    {0.3f, 0.3f},  // upper right
    {0.2f, 0.5f},  // lower left
    {0.3f, 0.5f},  // lower right
  };
  ptsWithBb.intrinsics = Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6);
  DetectedPoints ptsWithRoi;
  // Compute a detection ROI which functionally expands the box by 50%.
  ptsWithRoi.roi = detectionRoi(ptsWithBb);
  // Recover corners from the ROI.
  auto roiBox = roiCornersInImage(ptsWithRoi);
  EXPECT_THAT(roiBox[0].data(), equalsPoint(HPoint2{0.175f, 0.25f}));
  EXPECT_THAT(roiBox[1].data(), equalsPoint(HPoint2{0.175f, 0.55f}));  // { 0.175, 0.45 }
  EXPECT_THAT(roiBox[2].data(), equalsPoint(HPoint2{0.325f, 0.55f}));  // { 0.355278, 0.45 }
  EXPECT_THAT(roiBox[3].data(), equalsPoint(HPoint2{0.325f, 0.25f}));  // { 0.355278, 0.25 }
}

TEST_F(FaceGeometryTest, TestDetectionRoiNoPadding) {
  DetectedPoints ptsWithBb;
  ptsWithBb.intrinsics = Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6);
  ptsWithBb.boundingBox = {
    {0.2f, 0.3f},  // upper left
    {0.3f, 0.3f},  // upper right
    {0.2f, 0.5f},  // lower left
    {0.3f, 0.5f},  // lower right
  };
  DetectedPoints ptsWithRoi = ptsWithBb;
  // Compute a detection ROI without padding.
  ptsWithRoi.roi = detectionRoiNoPadding(ptsWithBb);
  // Recover corners from the ROI.
  auto roiBox = roiCornersInImage(ptsWithRoi);
  EXPECT_THAT(roiBox[0].data(), equalsPoint(HPoint2{0.2f, 0.3f}));
  EXPECT_THAT(roiBox[1].data(), equalsPoint(HPoint2{0.2f, 0.5f}));
  EXPECT_THAT(roiBox[2].data(), equalsPoint(HPoint2{0.3f, 0.5f}));
  EXPECT_THAT(roiBox[3].data(), equalsPoint(HPoint2{0.3f, 0.3f}));
}

TEST_F(FaceGeometryTest, TestDetectionToMeshNoFilter) {
  DetectedPoints a{
    1.0f,                                              // confidence
    0,                                                 // detectedClass
    {0, 0, 128, 128},                                  // viewport
    {ImageRoi::Source::FACE, 0, "", HMatrixGen::i()},  // roi
    {
      // boundingBox
      {.25f, 0.5f},  // upper left
      {.75f, 0.5f},  // upper right
      {.25f, 1.0f},  // lower left
      {.75f, 1.0f},  // lower right
    },
    FACEMESH_SAMPLE_OUTPUT,                                        // points
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6),  // intrinsics
  };

  ScopeTimer rt("test");
  const Face3d faceData = detectionToMeshNoFilter(a);
}

TEST_F(FaceGeometryTest, TestCreateAttachmentPoint) {
  ScopeTimer rt("test");
  Vector<HVector3> normals;
  computeVertexNormals(BLAZEFACE_SAMPLE_VERTICES, BLAZEFACE_INDICES, &normals);

  // left eye
  {
    auto pt = createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::LEFT_EYE,
      BLAZEFACE_L_EYE,
      BLAZEFACE_SAMPLE_VERTICES,
      normals);

    EXPECT_THAT(pt.position.data(), equalsPoint(BLAZEFACE_SAMPLE_VERTICES[BLAZEFACE_L_EYE]));
    HPoint3 up{0.0f, 1.0f, 0.0f};
    auto normalUp = pt.rotation.toRotationMat() * up;
    EXPECT_NEAR(normalUp.x(), normals[BLAZEFACE_L_EYE].x(), 1e-6);
    EXPECT_NEAR(normalUp.y(), normals[BLAZEFACE_L_EYE].y(), 1e-6);
    EXPECT_NEAR(normalUp.z(), normals[BLAZEFACE_L_EYE].z(), 1e-6);
  }
  // right eye
  {
    auto pt = createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::RIGHT_EYE,
      BLAZEFACE_R_EYE,
      BLAZEFACE_SAMPLE_VERTICES,
      normals);

    EXPECT_THAT(pt.position.data(), equalsPoint(BLAZEFACE_SAMPLE_VERTICES[BLAZEFACE_R_EYE]));
    HPoint3 up{0.0f, 1.0f, 0.0f};
    auto normalUp = pt.rotation.toRotationMat() * up;
    EXPECT_NEAR(normalUp.x(), normals[BLAZEFACE_R_EYE].x(), 1e-6);
    EXPECT_NEAR(normalUp.y(), normals[BLAZEFACE_R_EYE].y(), 1e-6);
    EXPECT_NEAR(normalUp.z(), normals[BLAZEFACE_R_EYE].z(), 1e-6);
  }

  // nose
  {
    auto pt = createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::NOSE_TIP,
      BLAZEFACE_NOSE,
      BLAZEFACE_SAMPLE_VERTICES,
      normals);

    EXPECT_THAT(pt.position.data(), equalsPoint(BLAZEFACE_SAMPLE_VERTICES[BLAZEFACE_NOSE]));
    HPoint3 up{0.0f, 1.0f, 0.0f};
    auto normalUp = pt.rotation.toRotationMat() * up;
    EXPECT_NEAR(normalUp.x(), normals[BLAZEFACE_NOSE].x(), 1e-6);
    EXPECT_NEAR(normalUp.y(), normals[BLAZEFACE_NOSE].y(), 1e-6);
    EXPECT_NEAR(normalUp.z(), normals[BLAZEFACE_NOSE].z(), 1e-6);
  }

  // mouth
  {
    auto pt = createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::MOUTH,
      BLAZEFACE_MOUTH,
      BLAZEFACE_SAMPLE_VERTICES,
      normals);

    EXPECT_THAT(pt.position.data(), equalsPoint(BLAZEFACE_SAMPLE_VERTICES[BLAZEFACE_MOUTH]));
    HPoint3 up{0.0f, 1.0f, 0.0f};
    auto normalUp = pt.rotation.toRotationMat() * up;
    EXPECT_NEAR(normalUp.x(), normals[BLAZEFACE_MOUTH].x(), 1e-6);
    EXPECT_NEAR(normalUp.y(), normals[BLAZEFACE_MOUTH].y(), 1e-6);
    EXPECT_NEAR(normalUp.z(), normals[BLAZEFACE_MOUTH].z(), 1e-6);
  }

  // left ear
  {
    auto pt = createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::LEFT_EAR,
      BLAZEFACE_L_EAR,
      BLAZEFACE_SAMPLE_VERTICES,
      normals);

    EXPECT_THAT(pt.position.data(), equalsPoint(BLAZEFACE_SAMPLE_VERTICES[BLAZEFACE_L_EAR]));
    HPoint3 up{0.0f, 1.0f, 0.0f};
    auto normalUp = pt.rotation.toRotationMat() * up;
    EXPECT_NEAR(normalUp.x(), normals[BLAZEFACE_L_EAR].x(), 1e-6);
    EXPECT_NEAR(normalUp.y(), normals[BLAZEFACE_L_EAR].y(), 1e-6);
    EXPECT_NEAR(normalUp.z(), normals[BLAZEFACE_L_EAR].z(), 1e-6);
  }

  // right ear
  {
    auto pt = createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::RIGHT_EAR,
      BLAZEFACE_R_EAR,
      BLAZEFACE_SAMPLE_VERTICES,
      normals);

    EXPECT_THAT(pt.position.data(), equalsPoint(BLAZEFACE_SAMPLE_VERTICES[BLAZEFACE_R_EAR]));
    HPoint3 up{0.0f, 1.0f, 0.0f};
    auto normalUp = pt.rotation.toRotationMat() * up;
    EXPECT_NEAR(normalUp.x(), normals[BLAZEFACE_R_EAR].x(), 1e-6);
    EXPECT_NEAR(normalUp.y(), normals[BLAZEFACE_R_EAR].y(), 1e-6);
    EXPECT_NEAR(normalUp.z(), normals[BLAZEFACE_R_EAR].z(), 1e-6);
  }
}

TEST_F(FaceGeometryTest, TestRenderTexToRenderTex) {
  // detections in originRoi with random rotation and translation
  ImageViewport viewport{0, 0, 256, 256};
  auto k = Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6);

  auto rand0to1 = []() { return static_cast<float>(rand() % 256) / 256.0f; };

  // construct original detection
  const float originRotRad = rand0to1() * M_PI;
  const float originCx = rand0to1();
  const float originCy = rand0to1();
  HMatrix originRot = HMatrixGen::zRadians(originRotRad);
  HMatrix originCenter = HMatrixGen::translation(-originCx, -originCy, 0.0f);

  DetectionRoi originRoi = {
    ImageRoi::Source::FACE,
    0,
    "",
    originRot * originCenter,
  };

  DetectedPoints detInOriginRoi{
    1,
    0,
    viewport,
    originRoi,
    {},  // bounding box -- will fill later.
    {},  // points -- will fill later.
    k,
  };
  for (size_t k = 0; k < 3; ++k) {
    detInOriginRoi.points.push_back({rand0to1(), rand0to1(), 1.0f});
  }

  // construct newRoi for new detection
  const float rotRad = rand0to1() * M_PI;
  const float cx = rand0to1();
  const float cy = rand0to1();
  HMatrix rotate = HMatrixGen::zRadians(rotRad);
  HMatrix center = HMatrixGen::translation(-cx, -cy, 0.0f);

  DetectionRoi newRoi = {
    ImageRoi::Source::FACE,
    0,
    "",
    rotate * center,
  };

  DetectedPoints detInNewRoi{
    1,
    0,
    viewport,
    newRoi,
    {},  // bounding box -- will fill later.
    {},  // points -- will fill later.
    k,
  };

  // transform points to newRoi
  HMatrix m = renderTexToRenderTex(originRoi, newRoi);
  detInNewRoi.points = m * detInOriginRoi.points;

  // compare results in image space
  DetectedImagePoints originImgPts = detectionToImageSpace(detInOriginRoi);
  DetectedImagePoints newImgPts = detectionToImageSpace(detInNewRoi);
  EXPECT_EQ(originImgPts.points.size(), newImgPts.points.size());

  for (size_t i = 0; i < originImgPts.points.size(); ++i) {
    auto &p0 = originImgPts.points[i];
    auto &p1 = newImgPts.points[i];
    EXPECT_FLOAT_EQ(p0.x(), p1.x());
    EXPECT_FLOAT_EQ(p0.y(), p1.y());
  }
}

}  // namespace c8
