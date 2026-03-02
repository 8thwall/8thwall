// Copyright (c) 2023 Niantic Inc
// Original Author: Dat Chu (datchu@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":face-tracker",
    "//c8:parameter-data",
    "//c8:string",
    "//c8/geometry:facemesh-data",
    "//c8/geometry:intrinsics",
    "//c8/io:file-io",
    "//c8/io:image-io",
    "//c8/pixels:draw2",
    "//c8/pixels:pixel-buffers",
    "//c8/pixels:pixel-transforms",
    "//reality/engine/ears:ear-types",
    "@com_google_googletest//:gtest_main",
    "@json",
  };
  data = {
    "//reality/engine/faces/testdata:ears-fidelity",
    "//reality/engine/testdata:realface",
    "//third_party/mediapipe/models:face-detection-front",
    "//third_party/mediapipe/models:face-landmark-attention",
  };
  linkstatic = 1;
}
cc_end(0x8c75a3b1);

#include <cmath>
#include <cstdio>
#include <fstream>
#include <nlohmann/json.hpp>

#include "c8/geometry/facemesh-data.h"
#include "c8/geometry/intrinsics.h"
#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/parameter-data.h"
#include "c8/pixels/draw2.h"
#include "c8/pixels/pixel-buffers.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/stats/scope-timer.h"
#include "c8/string.h"
#include "gtest/gtest.h"
#include "reality/engine/ears/ear-types.h"
#include "reality/engine/faces/face-tracker.h"

namespace c8 {
constexpr bool WRITE_IMAGE = false;

static constexpr char FACE_MODEL[] = "third_party/mediapipe/models/face_detection_front.tflite";
static constexpr char MESH_MODEL[] =
  "third_party/mediapipe/models/face_landmark_with_attention.tflite";
static constexpr char IMAGE_PATH[] = "reality/engine/testdata/real_test_face.jpg";
static constexpr char FIDELITY_CROPPING_JSON_PATH[] =
  "reality/engine/faces/testdata/fidelity_testing_cropping.json";
static constexpr char FIDELITY_LIFTING_JSON_PATH[] =
  "reality/engine/faces/testdata/fidelity_testing_lifting3d.json";

// https://docs.google.com/presentation/d/<REMOVED_BEFORE_OPEN_SOURCING>
struct EarPoints2 {
  HPoint2 topInnerCorner;
  HPoint2 topOuterCorner;
  HPoint2 bottomOuterCorner;
};

struct EarPoints3 {
  HPoint3 topInnerCorner;
  HPoint3 topOuterCorner;
  HPoint3 bottomOuterCorner;
};

struct CroppingTestData {
  Vector<HPoint3> inputLandmarks;
  EarPoints2 outputLeft;
  EarPoints2 outputRight;
};

template <size_t D>
struct EarLandmarks {
  HPoint<D> lobe;
  HPoint<D> canal;
  HPoint<D> helix;
};

using EarLandmarks2 = EarLandmarks<2>;
using EarLandmarks3 = EarLandmarks<3>;

struct LiftingTestData {
  Vector<HPoint3> inputLandmarks;
  EarLandmarks2 inputLeft;
  EarLandmarks2 inputRight;
  EarLandmarks3 outputLeft;
  EarLandmarks3 outputRight;
};

HPoint2 toHPoint2(const nlohmann::json &corner) { return {corner[0], corner[1]}; }

HPoint3 toHPoint3(const nlohmann::json &corner) { return {corner[0], corner[1], corner[2]}; }

HPoint2 uvToPixelCoord(const HPoint2 &uv, int width, int height) {
  return {uv.x() * (width - 1), uv.y() * (height - 1)};
}

// TODO(dat): Rename so ColorCombo can be used for ear landmarks, not just ear boxes.
struct ColorCombo {
  Color topInner;
  Color topOuter;
  Color bottomOuter;

  static ColorCombo set0() { return {Color::RED, Color::GREEN, Color::BLUE}; }
  static ColorCombo set1() { return {Color::DARK_RED, Color::DARK_GREEN, Color::DARK_BLUE}; }
};

void drawOutputLandmark(
  const EarPoints2 &left, const EarPoints2 &right, RGBA8888PlanePixels img, ColorCombo colors) {
  int width = img.cols();
  int height = img.rows();
  float size = 10.f;
  auto leftTopInnerCorner = uvToPixelCoord(left.topInnerCorner, width, height);
  auto leftTopOuterCorner = uvToPixelCoord(left.topOuterCorner, width, height);
  auto leftBottomOuterCorner = uvToPixelCoord(left.bottomOuterCorner, width, height);
  drawPoint(leftTopInnerCorner, size, colors.topInner, img);
  drawPoint(leftTopOuterCorner, size, colors.topOuter, img);
  drawPoint(leftBottomOuterCorner, size, colors.bottomOuter, img);
  drawLine(leftTopInnerCorner, leftTopOuterCorner, 1.f, Color::MINT, img);

  auto rightTopInnerCorner = uvToPixelCoord(right.topInnerCorner, width, height);
  auto rightTopOuterCorner = uvToPixelCoord(right.topOuterCorner, width, height);
  auto rightBottomOuterCorner = uvToPixelCoord(right.bottomOuterCorner, width, height);
  drawPoint(rightTopInnerCorner, size, colors.topInner, img);
  drawPoint(rightTopOuterCorner, size, colors.topOuter, img);
  drawPoint(rightBottomOuterCorner, size, colors.bottomOuter, img);
  drawLine(rightTopOuterCorner, rightBottomOuterCorner, 1.f, Color::MINT, img);
}

void drawInputLandmarks(
  const EarLandmarks2 &left,
  const EarLandmarks2 &right,
  RGBA8888PlanePixels img,
  ColorCombo colors) {
  int width = img.cols();
  int height = img.rows();
  float size = 10.f;
  auto leftLobe = uvToPixelCoord(left.lobe, width, height);
  auto leftCanal = uvToPixelCoord(left.canal, width, height);
  auto leftHelix = uvToPixelCoord(left.helix, width, height);
  drawPoint(leftLobe, size, colors.topInner, img);
  drawPoint(leftCanal, size, colors.topOuter, img);
  drawPoint(leftHelix, size, colors.bottomOuter, img);
  drawLine(leftLobe, leftCanal, 1.f, Color::MINT, img);

  auto rightLobe = uvToPixelCoord(right.lobe, width, height);
  auto rightCanal = uvToPixelCoord(right.canal, width, height);
  auto rightHelix = uvToPixelCoord(right.helix, width, height);
  drawPoint(rightLobe, size, colors.topInner, img);
  drawPoint(rightCanal, size, colors.topOuter, img);
  drawPoint(rightHelix, size, colors.bottomOuter, img);
  drawLine(rightCanal, rightHelix, 1.f, Color::MINT, img);
}

CroppingTestData readCroppingOutput(const String &fileName) {
  auto stream = std::ifstream(fileName);
  auto jsonData = nlohmann::json::parse(stream);

  CroppingTestData output;
  auto inputLandmarks = jsonData["input"]["mediapipe_3d_landmarks"];
  for (auto it = inputLandmarks.begin(); it != inputLandmarks.end(); it++) {
    output.inputLandmarks.push_back({(*it)[0], (*it)[1], (*it)[2]});
  }

  auto outputLeft = jsonData["output"]["models_left"];
  output.outputLeft = {
    toHPoint2(outputLeft["top_inner_corner"]),
    toHPoint2(outputLeft["top_outer_corner"]),
    toHPoint2(outputLeft["bottom_outer_corner"]),
  };

  auto outputRight = jsonData["output"]["models_right"];
  output.outputRight = {
    toHPoint2(outputRight["top_inner_corner"]),
    toHPoint2(outputRight["top_outer_corner"]),
    toHPoint2(outputRight["bottom_outer_corner"]),
  };

  return output;
}

LiftingTestData readLiftingOutput(const String &fileName) {
  auto stream = std::ifstream(fileName);
  auto jsonData = nlohmann::json::parse(stream);

  LiftingTestData output;
  auto inputLandmarks = jsonData["input"]["mediapipe_3d_landmarks"];
  for (auto it = inputLandmarks.begin(); it != inputLandmarks.end(); it++) {
    output.inputLandmarks.push_back({(*it)[0], (*it)[1], (*it)[2]});
  }
  auto inputLeft = jsonData["input"]["ear_2d_landmarks"]["models_left"];
  output.inputLeft = {
    toHPoint2(inputLeft[0]),
    toHPoint2(inputLeft[1]),
    toHPoint2(inputLeft[2]),
  };
  auto inputRight = jsonData["input"]["ear_2d_landmarks"]["models_right"];
  output.inputRight = {
    toHPoint2(inputRight[0]),
    toHPoint2(inputRight[1]),
    toHPoint2(inputRight[2]),
  };

  auto outputLeft = jsonData["output"]["ear_3d_landmarks"]["models_left"];
  output.outputLeft = {
    toHPoint3(outputLeft[0]),
    toHPoint3(outputLeft[1]),
    toHPoint3(outputLeft[2]),
  };
  auto outputRight = jsonData["output"]["ear_3d_landmarks"]["models_right"];
  output.outputRight = {
    toHPoint3(outputRight[0]),
    toHPoint3(outputRight[1]),
    toHPoint3(outputRight[2]),
  };

  return output;
}

class EarTrackerTest : public ::testing::Test {
protected:
  void SetUp() override {
    globalParams().set("EarTypes.filterEarAnchors", false);
    globalParams().set("EarTypes.earLandmarkDetectionCropAlpha", 0.6f);
  }
};

DetectedPoints makeFaceDetectedPoints(const Vector<HPoint3> &landmarks, int width, int height) {
  auto intrinsic = Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_13);
  auto scaledIntrinsic = Intrinsics::rotateCropAndScaleIntrinsics(intrinsic, width, height);
  DetectedPoints d;
  d.points = landmarks;
  d.boundingBox = {
    {0.0f, 0.0f},  // upper left
    {1.0f, 0.0f},  // upper right
    {0.0f, 1.0f},  // lower left
    {1.0f, 1.0f},  // lower right
  };
  d.roi = {
    ImageRoi::Source::FACE,
    1,   // faceId
    "",  // name
    // warp. Coordinates in the JSON file is wrt to the entire image. Warp is identity.
    HMatrixGen::i(),
    {},                                      // geom
    HMatrixGen::intrinsic(scaledIntrinsic),  // intrinsics
    HMatrixGen::i(),                         // globalPose
  };
  // Reflect that the entire image is used for input inference.
  d.viewport = {0, 0, static_cast<float>(width), static_cast<float>(height)};
  d.intrinsics = scaledIntrinsic;  // Not used in earRoisByFaceMesh
  return d;
}

void addEarDetectedPtsInfo(
  DetectedPoints *d,
  const ImageRoi::Source &earSrc,
  const c8_PixelPinholeCameraModel &intrinsics,
  int width,
  int height) {
  d->roi = {
    earSrc,
    1,   // faceId
    "",  // name
    // warp. Coordinates in the JSON file is wrt to the entire image. Warp is identity.
    HMatrixGen::i(),
    {},                                 // geom
    HMatrixGen::intrinsic(intrinsics),  // intrinsics
    HMatrixGen::i(),                    // globalPose
  };
  // Reflect that the entire image is used for input inference.
  d->viewport = {0, 0, static_cast<float>(width), static_cast<float>(height)};
  d->intrinsics = intrinsics;
}

void checkPoint(const HPoint2 &expected, const HPoint2 &actual) {
  EXPECT_NEAR(expected.x(), actual.x(), 0.0001f);
  EXPECT_NEAR(expected.y(), actual.y(), 0.0001f);
}

void checkPoint(const HPoint3 &expected, const HPoint3 &actual) {
  EXPECT_NEAR(expected.x(), actual.x(), 0.0001f);
  EXPECT_NEAR(expected.y(), actual.y(), 0.0001f);
  EXPECT_NEAR(expected.z(), actual.z(), 0.0001f);
}

TEST_F(EarTrackerTest, TestCroppingImage) {
  auto fidelityData = readCroppingOutput(FIDELITY_CROPPING_JSON_PATH);
  EXPECT_EQ(478, fidelityData.inputLandmarks.size());
  EXPECT_FLOAT_EQ(0.39597558975219727, fidelityData.inputLandmarks[0].x());
  EXPECT_FLOAT_EQ(0.5725057125091553, fidelityData.inputLandmarks[0].y());
  EXPECT_FLOAT_EQ(-0.03312905505299568, fidelityData.inputLandmarks[0].z());

  EXPECT_FLOAT_EQ(0.6523301601409912, fidelityData.outputLeft.topInnerCorner.x());
  EXPECT_FLOAT_EQ(0.38380688428878784, fidelityData.outputLeft.topInnerCorner.y());
  EXPECT_FLOAT_EQ(0.8543052673339844, fidelityData.outputLeft.topOuterCorner.x());
  EXPECT_FLOAT_EQ(0.3972854018211365, fidelityData.outputLeft.topOuterCorner.y());
  EXPECT_FLOAT_EQ(0.8286290764808655, fidelityData.outputLeft.bottomOuterCorner.x());
  EXPECT_FLOAT_EQ(0.6136627793312073, fidelityData.outputLeft.bottomOuterCorner.y());

  EXPECT_FLOAT_EQ(0.42510518431663513, fidelityData.outputRight.topInnerCorner.x());
  EXPECT_FLOAT_EQ(0.3825589120388031, fidelityData.outputRight.topInnerCorner.y());
  EXPECT_FLOAT_EQ(0.24944227933883667, fidelityData.outputRight.topOuterCorner.x());
  EXPECT_FLOAT_EQ(0.3979725241661072, fidelityData.outputRight.topOuterCorner.y());
  EXPECT_FLOAT_EQ(0.2788049280643463, fidelityData.outputRight.bottomOuterCorner.x());
  EXPECT_FLOAT_EQ(0.5861614942550659, fidelityData.outputRight.bottomOuterCorner.y());
  // Cropping: given a set of 3d landmarks from media pipes, provide 3 corners per ear

  // The input image used for fidelity testing. Already in 3/4 aspect ratio.
  auto inputImgBuffer = readImageToRGBA(IMAGE_PATH);
  auto faceDetectedPts = makeFaceDetectedPoints(
    fidelityData.inputLandmarks, inputImgBuffer.pixels().cols(), inputImgBuffer.pixels().rows());
  EarBoundingBoxDebug earBoundingBoxes;
  auto earRois = earRoisByFaceMesh(faceDetectedPts, &earBoundingBoxes);
  EXPECT_EQ(2, earRois.size());

  // Only available via the debug output
  // Commented out right now until we have a way to set engine's crop alpha to 0.6
  checkPoint(fidelityData.outputLeft.topInnerCorner, earBoundingBoxes.left.upperLeft);
  checkPoint(fidelityData.outputLeft.topOuterCorner, earBoundingBoxes.left.upperRight);
  checkPoint(fidelityData.outputLeft.bottomOuterCorner, earBoundingBoxes.left.lowerRight);

  if (WRITE_IMAGE) {
    auto debugImgBuffer = clonePixels(inputImgBuffer.pixels());
    drawOutputLandmark(
      fidelityData.outputLeft,
      fidelityData.outputRight,
      debugImgBuffer.pixels(),
      ColorCombo::set0());

    EarPoints2 debugLeft{
      earBoundingBoxes.left.upperLeft,
      earBoundingBoxes.left.upperRight,
      earBoundingBoxes.left.lowerRight};
    EarPoints2 debugRight{
      earBoundingBoxes.right.upperRight,
      earBoundingBoxes.right.upperLeft,
      earBoundingBoxes.right.lowerLeft};
    drawOutputLandmark(debugLeft, debugRight, debugImgBuffer.pixels(), ColorCombo::set1());
    writeImage(debugImgBuffer.pixels(), "/tmp/cropping_debug.jpg");
  }
}

DetectedPoints makeEarDetectedPoints(
  const HPoint3 &faceLandmarkEarTop,
  const HPoint3 &faceLandmarkEarLow,
  const EarLandmarks2 &earLandmarks,
  bool visible) {
  // We store in Vector<HPoint3> but it's only 2D
  DetectedPoints earPts;
  // In the engine, we encode visible in the Z channel
  float visibleZ = visible ? 1.0f : 0.0f;
  earPts.points = {
    {faceLandmarkEarTop.x(), faceLandmarkEarTop.y(), 1.f},
    {faceLandmarkEarLow.x(), faceLandmarkEarTop.y(), 1.f},
    {earLandmarks.lobe.x(), earLandmarks.lobe.y(), visibleZ},
    {earLandmarks.canal.x(), earLandmarks.canal.y(), visibleZ},
    {earLandmarks.helix.x(), earLandmarks.helix.y(), visibleZ},
  };
  return earPts;
}

Face3d makeFace(const DetectedPoints &localFace) {
  // TrackedFaceState faceState; // a new face state since we are seeing this face for the 1st time
  // Face3d face = faceState.locateFace(localFace);
  Face3d face;
  face.vertices = localFace.points;
  face.status = Face3d::TrackingStatus::FOUND;
  face.transform = {{}, {}, 1.f, 1.f, 1.f, 1.f};  // identity face anchor transform
  return face;
}

TEST_F(EarTrackerTest, TestLiftingImage) {
  auto fidelityData = readLiftingOutput(FIDELITY_LIFTING_JSON_PATH);
  EXPECT_EQ(478, fidelityData.inputLandmarks.size());
  EXPECT_FLOAT_EQ(0.39597558975219727, fidelityData.inputLandmarks[0].x());
  EXPECT_FLOAT_EQ(0.5725057125091553, fidelityData.inputLandmarks[0].y());
  EXPECT_FLOAT_EQ(-0.03312905505299568, fidelityData.inputLandmarks[0].z());

  EXPECT_FLOAT_EQ(0.7294090390205383, fidelityData.inputLeft.lobe.x());
  EXPECT_FLOAT_EQ(0.5055955648422241, fidelityData.inputLeft.lobe.y());
  EXPECT_FLOAT_EQ(0.7329992651939392, fidelityData.inputLeft.canal.x());
  EXPECT_FLOAT_EQ(0.4753403961658478, fidelityData.inputLeft.canal.y());
  EXPECT_FLOAT_EQ(0.7792000770568848, fidelityData.inputLeft.helix.x());
  EXPECT_FLOAT_EQ(0.4326813817024231, fidelityData.inputLeft.helix.y());

  EXPECT_FLOAT_EQ(0.379135400056839, fidelityData.inputRight.lobe.x());
  EXPECT_FLOAT_EQ(0.5241878628730774, fidelityData.inputRight.lobe.y());
  EXPECT_FLOAT_EQ(0.2646292448043823, fidelityData.inputRight.canal.x());
  EXPECT_FLOAT_EQ(0.40531113743782043, fidelityData.inputRight.canal.y());
  EXPECT_FLOAT_EQ(0.3609020411968231, fidelityData.inputRight.helix.x());
  EXPECT_FLOAT_EQ(0.4457659125328064, fidelityData.inputRight.helix.y());

  // Assuming that filter is already off in our test SetUp()

  // The input image used for fidelity testing. Already in 3/4 aspect ratio.
  auto inputImgBuffer = readImageToRGBA(IMAGE_PATH);
  auto testImgWidth = inputImgBuffer.pixels().cols();
  auto testImgHeight = inputImgBuffer.pixels().rows();
  auto localFace = makeFaceDetectedPoints(
    fidelityData.inputLandmarks, inputImgBuffer.pixels().cols(), inputImgBuffer.pixels().rows());

  DetectedPoints leftEar = makeEarDetectedPoints(
    fidelityData.inputLandmarks[FACEMESH_L_EAR],
    fidelityData.inputLandmarks[FACEMESH_L_EAR_LOW],
    fidelityData.inputLeft,
    // left ear is visible
    true);

  DetectedPoints rightEar = makeEarDetectedPoints(
    fidelityData.inputLandmarks[FACEMESH_R_EAR],
    fidelityData.inputLandmarks[FACEMESH_R_EAR_LOW],
    fidelityData.inputRight,
    // right ear is not visible
    false);

  // Make an intrinsic that fits the test image
  auto intrinsic = Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_13);
  auto scaledIntrinsic =
    Intrinsics::rotateCropAndScaleIntrinsics(intrinsic, testImgWidth, testImgHeight);
  addEarDetectedPtsInfo(
    &leftEar, ImageRoi::Source::EAR_LEFT, scaledIntrinsic, testImgWidth, testImgHeight);
  addEarDetectedPtsInfo(
    &rightEar, ImageRoi::Source::EAR_RIGHT, scaledIntrinsic, testImgWidth, testImgHeight);

  Face3d face = makeFace(localFace);
  auto ear3d = earLiftLandmarksTo3D(localFace, leftEar, rightEar, face, scaledIntrinsic);
  // Assert these sizes before we delve into their values
  // Lifting does not compute attachment points
  ASSERT_EQ(0, ear3d.leftAttachmentPoints.size());
  ASSERT_EQ(0, ear3d.rightAttachmentPoints.size());

  ASSERT_EQ(3, ear3d.leftVertices.size());
  ASSERT_EQ(3, ear3d.leftVisibilities.size());

  ASSERT_EQ(3, ear3d.rightVertices.size());
  ASSERT_EQ(3, ear3d.rightVisibilities.size());

  // TODO(dat): Check output vertices

  // Check visibilities
  // Left ear is visible in the test image
  EXPECT_FLOAT_EQ(1.f, ear3d.leftVisibilities[0]);
  EXPECT_FLOAT_EQ(1.f, ear3d.leftVisibilities[1]);
  EXPECT_FLOAT_EQ(1.f, ear3d.leftVisibilities[2]);

  // Right ear is not visible
  EXPECT_FLOAT_EQ(0.f, ear3d.rightVisibilities[0]);
  EXPECT_FLOAT_EQ(0.f, ear3d.rightVisibilities[1]);
  EXPECT_FLOAT_EQ(0.f, ear3d.rightVisibilities[2]);

  if (WRITE_IMAGE) {
    auto debugImgBuffer = clonePixels(inputImgBuffer.pixels());
    drawInputLandmarks(
      fidelityData.inputLeft, fidelityData.inputRight, debugImgBuffer.pixels(), ColorCombo::set0());

    writeImage(debugImgBuffer.pixels(), "/tmp/lifting_debug.jpg");
  }
}

}  // namespace c8
