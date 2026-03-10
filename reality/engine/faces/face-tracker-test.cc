// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":face-tracker",
    "//c8:string",
    "//c8/geometry:facemesh-data",
    "//c8/geometry:intrinsics",
    "//c8/io:file-io",
    "//c8/io:image-io",
    "//c8/pixels:pixel-transforms",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//reality/engine/faces/testdata:roi",
    "//third_party/mediapipe/models:face-detection-front",
    "//third_party/mediapipe/models:face-landmark-attention",
  };
  linkstatic = 1;
}
cc_end(0x57483335);

#include <cmath>
#include <cstdio>

#include "c8/geometry/facemesh-data.h"
#include "c8/geometry/intrinsics.h"
#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/stats/scope-timer.h"
#include "c8/string.h"
#include "gtest/gtest.h"
#include "reality/engine/faces/face-tracker.h"

namespace c8 {

class FaceTrackerTest : public ::testing::Test {};

static constexpr char FACE_MODEL[] = "third_party/mediapipe/models/face_detection_front.tflite";
static constexpr char MESH_MODEL[] =
  "third_party/mediapipe/models/face_landmark_with_attention.tflite";
static constexpr char IMAGE_PATH[] = "reality/engine/faces/testdata/roi.jpg";

TEST_F(FaceTrackerTest, TestRecordFaceBBoxInRay) {
  // Box rotated 45 degrees clockwise.
  DetectedPoints globalA;
  globalA.boundingBox = {
    {0.5f, 1.f},  // upper left
    {1.f, 0.5f},  // upper right
    {0.f, 0.5f},  // lower left
    {0.5f, 0.f},  // lower right
  };
  globalA.roi.faceId = 2;

  TreeMap<int, BoundingBox> faceToUprightBBoxInRay;

  recordFaceBBoxInRay({globalA}, &faceToUprightBBoxInRay);

  EXPECT_FLOAT_EQ(faceToUprightBBoxInRay.at(2).upperLeft.x(), -1.f);
  EXPECT_FLOAT_EQ(faceToUprightBBoxInRay.at(2).upperLeft.y(), 0.f);
  EXPECT_FLOAT_EQ(faceToUprightBBoxInRay.at(2).upperRight.x(), 0.f);
  EXPECT_FLOAT_EQ(faceToUprightBBoxInRay.at(2).upperRight.y(), 0.f);
  EXPECT_FLOAT_EQ(faceToUprightBBoxInRay.at(2).lowerLeft.x(), -1.f);
  EXPECT_FLOAT_EQ(faceToUprightBBoxInRay.at(2).lowerLeft.y(), -1.f);
  EXPECT_FLOAT_EQ(faceToUprightBBoxInRay.at(2).lowerRight.x(), 0.f);
  EXPECT_FLOAT_EQ(faceToUprightBBoxInRay.at(2).lowerRight.y(), -1.f);
}

TEST_F(FaceTrackerTest, TestFilterGlobalAndAssignIdsEmpty) {
  const Vector<DetectedPoints> globalDetections;
  const Vector<DetectedPoints> previousGlobalDetections;
  TreeMap<int, TrackedFaceState> facesById;
  int maxTrackedFaces = 1;
  int currentFrame = 1;
  TreeMap<int, BoundingBox> faceToUprightBBoxInRay;
  TreeMap<int, int> faceIdToFirstFrame;
  auto filteredGlobal = filterGlobalAndAssignIds(
    globalDetections,
    facesById,
    previousGlobalDetections,
    maxTrackedFaces,
    currentFrame,
    &faceToUprightBBoxInRay,
    &faceIdToFirstFrame);
  EXPECT_TRUE(filteredGlobal.empty());
  EXPECT_TRUE(faceToUprightBBoxInRay.empty());
}

TEST_F(FaceTrackerTest, TestFilterGlobalAndAssignIdsRedundantGlobal) {
  // This will be a new global with faceId 2.
  DetectedPoints globalA;
  globalA.boundingBox = {
    {0.5f, 1.f},  // upper left
    {1.f, 0.5f},  // upper right
    {0.f, 0.5f},  // lower left
    {0.5f, 0.f},  // lower right
  };

  BoundingBox bboxB = {
    {-0.5f, 1.f},  // upper left
    {0.f, 0.5f},   // upper right
    {-1.f, 0.5f},  // lower left
    {-0.5f, 0.f},  // lower right
  };
  DetectedPoints globalB;
  globalB.boundingBox = bboxB;

  // globalB is redundant of localB.
  DetectedPoints localB;
  localB.boundingBox = bboxB;
  localB.roi.faceId = 1;

  const Vector<DetectedPoints> globalDetections = {globalA, globalB};
  const Vector<DetectedPoints> previousGlobalDetections;

  TreeMap<int, TrackedFaceState> facesById;
  // facesById.insert({1, {}});
  // Creates a new one.
  facesById[1];

  int maxTrackedFaces = 2;
  int currentFrame = 2;
  TreeMap<int, BoundingBox> faceToUprightBBoxInRay;
  TreeMap<int, int> faceIdToFirstFrame;

  recordFaceBBoxInRay({localB}, &faceToUprightBBoxInRay);
  auto filteredGlobal = filterGlobalAndAssignIds(
    globalDetections,
    facesById,
    previousGlobalDetections,
    maxTrackedFaces,
    currentFrame,
    &faceToUprightBBoxInRay,
    &faceIdToFirstFrame);
  EXPECT_EQ(filteredGlobal.size(), 1);
  EXPECT_EQ(filteredGlobal[0].roi.faceId, 2);
  EXPECT_EQ(faceToUprightBBoxInRay.size(), 2);
}

TEST_F(FaceTrackerTest, TestFilterGlobalAndAssignIdsTestNewGlobal) {
  DetectedPoints globalA;
  globalA.boundingBox = {
    {0.5f, 1.f},  // upper left
    {1.f, 0.5f},  // upper right
    {0.f, 0.5f},  // lower left
    {0.5f, 0.f},  // lower right
  };

  DetectedPoints globalB;
  globalB.boundingBox = {
    {-0.5f, 1.f},  // upper left
    {0.f, 0.5f},   // upper right
    {-1.f, 0.5f},  // lower left
    {-0.5f, 0.f},  // lower right
  };
  const Vector<DetectedPoints> globalDetections = {globalA, globalB};
  const Vector<DetectedPoints> previousGlobalDetections;

  TreeMap<int, TrackedFaceState> facesById;
  int maxTrackedFaces = 1;
  int currentFrame = 1;
  TreeMap<int, BoundingBox> faceToUprightBBoxInRay;
  TreeMap<int, int> faceIdToFirstFrame;

  auto filteredGlobal = filterGlobalAndAssignIds(
    globalDetections,
    facesById,
    previousGlobalDetections,
    maxTrackedFaces,
    currentFrame,
    &faceToUprightBBoxInRay,
    &faceIdToFirstFrame);
  EXPECT_EQ(filteredGlobal.size(), 2);
  EXPECT_EQ(filteredGlobal[0].roi.faceId, 1);
  EXPECT_EQ(filteredGlobal[1].roi.faceId, 2);
  EXPECT_EQ(faceToUprightBBoxInRay.size(), 2);
}

TEST_F(FaceTrackerTest, TestFilterGlobalAndAssignIdsTestMissedGlobalNPlus1) {
  // Tests that we copy over a missed global detection.
  DetectedPoints globalA;
  globalA.boundingBox = {
    {0.5f, 1.f},  // upper left
    {1.f, 0.5f},  // upper right
    {0.f, 0.5f},  // lower left
    {0.5f, 0.f},  // lower right
  };

  // THIS is the one we miss on N+1 but find on N.
  DetectedPoints globalB;
  globalB.boundingBox = {
    {-0.5f, 1.f},  // upper left
    {0.f, 0.5f},   // upper right
    {-1.f, 0.5f},  // lower left
    {-0.5f, 0.f},  // lower right
  };
  const Vector<DetectedPoints> globalDetections = {globalA};
  Vector<DetectedPoints> previousGlobalDetections = {globalA, globalB};
  previousGlobalDetections.front().roi.faceId = 1;
  previousGlobalDetections.back().roi.faceId = 2;

  TreeMap<int, TrackedFaceState> facesById;
  int maxTrackedFaces = 3;
  int currentFrame = 1;
  TreeMap<int, BoundingBox> faceToUprightBBoxInRay;
  TreeMap<int, int> faceIdToFirstFrame = {{0, 1}, {0, 2}};

  auto filteredGlobal = filterGlobalAndAssignIds(
    globalDetections,
    facesById,
    previousGlobalDetections,
    maxTrackedFaces,
    currentFrame,
    &faceToUprightBBoxInRay,
    &faceIdToFirstFrame);
  EXPECT_EQ(filteredGlobal.size(), 2);
  EXPECT_EQ(filteredGlobal[0].roi.faceId, 1);
  // We got globalB back even though we didn't find it this frame.
  EXPECT_EQ(filteredGlobal[1].roi.faceId, 2);
}

TEST_F(FaceTrackerTest, TestFilterGlobalAndAssignIdsTestMissedGlobalNPlus2) {
  // Tests that we copy over a missed global detection.
  DetectedPoints globalA;
  globalA.boundingBox = {
    {0.5f, 1.f},  // upper left
    {1.f, 0.5f},  // upper right
    {0.f, 0.5f},  // lower left
    {0.5f, 0.f},  // lower right
  };

  // THIS is the one we find on N+1 but miss on N. We should copy it over for N+2.
  DetectedPoints globalB;
  globalB.boundingBox = {
    {-0.5f, 1.f},  // upper left
    {0.f, 0.5f},   // upper right
    {-1.f, 0.5f},  // lower left
    {-0.5f, 0.f},  // lower right
  };
  const Vector<DetectedPoints> globalDetections;
  Vector<DetectedPoints> previousGlobalDetections = {globalA, globalB};
  previousGlobalDetections.front().roi.faceId = 1;
  previousGlobalDetections.back().roi.faceId = 2;

  // We only have local detections for globalA, not globalB. Let's copy it over.
  TreeMap<int, TrackedFaceState> facesById;
  facesById[1];
  int maxTrackedFaces = 3;
  int currentFrame = 2;
  TreeMap<int, BoundingBox> faceToUprightBBoxInRay;
  // We missed id 2 on the first frame, but found it on the second.
  TreeMap<int, int> faceIdToFirstFrame = {{1, 0}, {2, 1}};

  auto filteredGlobal = filterGlobalAndAssignIds(
    globalDetections,
    facesById,
    previousGlobalDetections,
    maxTrackedFaces,
    currentFrame,
    &faceToUprightBBoxInRay,
    &faceIdToFirstFrame);
  EXPECT_EQ(filteredGlobal.size(), 1);
  // We got globalB back even though we didn't find it this frame.
  EXPECT_EQ(filteredGlobal[0].roi.faceId, 2);
}

TEST_F(FaceTrackerTest, TestMesh) {
  auto im = readImageToRGBA(IMAGE_PATH);
  auto pix = im.pixels();

  auto k = Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6);

  // Values extracted from face-detect.cc
  ImageRoi fullImageRoi{ImageRoi::Source::FACE, 0, "", HMatrixGen::i()};
  ImageRoi meshCrop0{
    ImageRoi::Source::FACE,
    1,
    "",
    {
      // warp
      {3.964915, 0.000000, 0.000000, 2.257771},
      {0.000000, 5.286576, 0.000000, 1.788887},
      {0.000000, 0.000000, 1.000000, 0.000000},
      {0.000000, 0.000000, 0.000000, 1.000000},
    }};
  ImageRoi meshCrop1{
    ImageRoi::Source::FACE,
    2,
    "",
    {
      // roi
      {3.631096, 0.000000, 0.000000, -1.445287},
      {0.000000, 4.841462, 0.000000, 2.579028},
      {0.000000, 0.000000, 1.000000, 0.000000},
      {0.000000, 0.000000, 0.000000, 1.000000},
    }};

  FaceRenderResult r{
    0,    // srcTex,
    pix,  // rawPixels,
    {
      // faceDetectImage
      {16.0f, 0.0f, 96.0f, 128.0f},  // viewport
      crop(pix, 128, 96, 0, 16),     // image
      fullImageRoi,                  // roi
    },
    {// faceMeshImages
     {
       // 0
       {192.0f, 0.0f, 192.0f, 192.0f},  // viewport
       crop(pix, 192, 192, 0, 192),     // image
       meshCrop0,
     },
     {
       // 2
       {0.0f, 192.0f, 192.0f, 192.0f},  // viewport
       crop(pix, 192, 192, 192, 0),     // image
       meshCrop1,
     }},
  };

  FaceTracker::setMaxTrackedFaces(3);
  FaceTracker t;

  t.setFaceDetectModel(readFile(FACE_MODEL));
  t.setFaceMeshModel(readFile(MESH_MODEL));

  ScopeTimer rt("test");
  auto result = t.track(r, k);

  EXPECT_EQ(2, result.localFaces->size());
  EXPECT_EQ(478, result.localFaces->at(0).points.size());

  // Since we didn't enable ears, local faces should not output EAR rois
  for (const auto &localFace: *result.localFaces) {
    auto roi = localFace.roi;
    EXPECT_NE(ImageRoi::Source::EAR_LEFT, roi.source);
    EXPECT_NE(ImageRoi::Source::EAR_RIGHT, roi.source);
  }

  EXPECT_EQ(2, result.faceData->size());
  EXPECT_EQ(478, result.faceData->at(0).vertices.size());
  EXPECT_EQ(1, result.faceData->at(0).id);
  EXPECT_EQ(Face3d::TrackingStatus::FOUND, result.faceData->at(0).status);
  EXPECT_EQ(478, result.faceData->at(1).vertices.size());
  EXPECT_EQ(478, result.faceData->at(1).vertices.size());
  EXPECT_EQ(2, result.faceData->at(1).id);
  EXPECT_EQ(Face3d::TrackingStatus::FOUND, result.faceData->at(1).status);

  // Run tracking again, the status should changed to UPDATED.
  result = t.track(r, k);
  EXPECT_EQ(2, result.faceData->size());
  EXPECT_EQ(478, result.faceData->at(0).vertices.size());
  EXPECT_EQ(1, result.faceData->at(0).id);
  EXPECT_EQ(Face3d::TrackingStatus::UPDATED, result.faceData->at(0).status);
  EXPECT_EQ(478, result.faceData->at(1).vertices.size());
  EXPECT_EQ(478, result.faceData->at(1).vertices.size());
  EXPECT_EQ(2, result.faceData->at(1).id);
  EXPECT_EQ(Face3d::TrackingStatus::UPDATED, result.faceData->at(1).status);

  // Artificially set the first ROI to read from a black region of the rendered image. This should
  // cause face detection to fail. There should only be one tracked face with id 2, and the first
  // face should be lost.
  r.faceMeshImages[0].image = crop(pix, 192, 192, 192, 192);
  result = t.track(r, k);
  EXPECT_EQ(2, result.faceData->size());
  EXPECT_EQ(478, result.faceData->at(0).vertices.size());
  EXPECT_EQ(2, result.faceData->at(0).id);
  EXPECT_EQ(Face3d::TrackingStatus::UPDATED, result.faceData->at(0).status);
  EXPECT_EQ(0, result.faceData->at(1).vertices.size());
  EXPECT_EQ(1, result.faceData->at(1).id);
  EXPECT_EQ(Face3d::TrackingStatus::LOST, result.faceData->at(1).status);
  EXPECT_EQ(1, result.lostIds->size());
  EXPECT_EQ(1, result.lostIds->at(0));

  // If we run with the same input a second time, we are still tracking #2, but the lost one is
  // no longer marked.
  result = t.track(r, k);
  EXPECT_EQ(1, result.faceData->size());
  EXPECT_EQ(478, result.faceData->at(0).vertices.size());
  EXPECT_EQ(2, result.faceData->at(0).id);
  EXPECT_EQ(Face3d::TrackingStatus::UPDATED, result.faceData->at(0).status);
  EXPECT_EQ(0, result.lostIds->size());
}

TEST_F(FaceTrackerTest, TestFaces) {
  // Read in a (slightly too big) image. The test image is 244x128, we need 128x128.
  auto im = readImageToRGBA(IMAGE_PATH);
  auto pix = im.pixels();

  auto k = Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6);

  // Values extracted from face-detect.cc
  FaceRenderResult r{
    0,    // srcTex,
    pix,  // rawPixels,
    {
      // faceDetectImage
      {16.0f, 0.0f, 96.0f, 128.0f},                      // viewport
      crop(pix, 128, 96, 0, 16),                         // image
      {ImageRoi::Source::FACE, 0, "", HMatrixGen::i()},  // roi
    },
    {},  // faceMeshImages
  };

  FaceTracker::setMaxTrackedFaces(3);
  FaceTracker t;

  t.setFaceDetectModel(readFile(FACE_MODEL));
  t.setFaceMeshModel(readFile(MESH_MODEL));

  ScopeTimer rt("test");
  auto result = t.track(r, k);

  EXPECT_EQ(2, result.globalFaces->size());
  EXPECT_EQ(6, result.globalFaces->at(0).points.size());
  EXPECT_EQ(0, result.localFaces->size());
  EXPECT_EQ(0, result.faceData->size());
}

}  // namespace c8
