// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":face-detector-local",
    "//c8:string",
    "//c8/camera:device-infos",
    "//c8/geometry:intrinsics",
    "//c8/io:file-io",
    "//c8/io:image-io",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//reality/engine/deepnets/testdata:crop",
    "//third_party/mediapipe/models:face-landmark-attention",
  };
  linkstatic = 1;
}
cc_end(0x1613d61b);

#include <cmath>
#include <cstdio>

#include "c8/camera/device-infos.h"
#include "c8/geometry/intrinsics.h"
#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/stats/scope-timer.h"
#include "c8/string.h"
#include "gtest/gtest.h"
#include "reality/engine/faces/face-detector-local.h"

namespace c8 {

class FaceDetectorLocalTest : public ::testing::Test {};

static constexpr char MODEL_PATH[] = "third_party/mediapipe/models/face_landmark_with_attention.tflite";
static constexpr char IMAGE_PATH[] = "reality/engine/deepnets/testdata/crop.jpg";

TEST_F(FaceDetectorLocalTest, TestAnaylyze) {
  // Read in a (slightly too big) image. The test image is 244x128, we need 128x128.
  auto im = readImageToRGBA(IMAGE_PATH);
  auto pix = im.pixels();
  auto k = Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6);

  // Take a hardcoded crop of the test image with a well centered face.

  RenderedSubImage img{{0, 0, 192, 192},
                       pix,
                       {
                         ImageRoi::Source::FACE,
                         0,
                         "",
                         HMatrixGen::i(),
                       }};

  // by default we don't do ears
  EarConfig earConfig;
  // Load a TFLite interpreter with the tensor flow model.
  FaceDetectorLocal mesher(readFile(MODEL_PATH), earConfig);

  ScopeTimer rt("test");
  auto faces = mesher.analyzeFace(img, k);

  EXPECT_EQ(1, faces.size());
  bool foundFace = false;
  EXPECT_EQ(478, faces[0].points.size());
  for (const auto &face: faces) {
    auto roi = face.roi;
    if (roi.source == ImageRoi::Source::FACE) {
      foundFace = true;
    }
    EXPECT_NE(ImageRoi::Source::EAR_LEFT, roi.source);
    EXPECT_NE(ImageRoi::Source::EAR_RIGHT, roi.source);
  }
  EXPECT_TRUE(foundFace) << "Should have found a face in our detection";
}

}  // namespace c8
