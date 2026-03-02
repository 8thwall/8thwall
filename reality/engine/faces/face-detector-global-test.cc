// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":face-detector-global",
    "//c8:string",
    "//c8/io:file-io",
    "//c8/io:image-io",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//reality/engine/deepnets/testdata:faces",
    "//third_party/mediapipe/models:face-detection-front",
  };
  linkstatic=1;
}
cc_end(0xfea29787);

#include <cmath>
#include <cstdio>

#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/string.h"
#include "gtest/gtest.h"
#include "reality/engine/faces/face-detector-global.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

class FaceDetectorGlobalTest : public ::testing::Test {};

static constexpr char MODEL_PATH[] = "third_party/mediapipe/models/face_detection_front.tflite";
static constexpr char IMAGE_PATH[] = "reality/engine/deepnets/testdata/faces.jpg";

TEST_F(FaceDetectorGlobalTest, TestDetect) {
  // Read in a (slightly too big) image. The test image is 244x128, we need 128x128.
  auto im = readImageToRGBA(IMAGE_PATH);
  auto pix = im.pixels();

  // Take a hardcoded crop of the test image with a well centered face.
  int cropRowStart = 0;
  int cropColStart = 110;

  RenderedSubImage img{
    {0, 0, 128, 128},
    {128, 128, pix.rowBytes(), pix.pixels() + cropRowStart * pix.rowBytes() + cropColStart * 4},
    {ImageRoi::Source::FACE, 0, "", HMatrixGen::i()},
  };

  // Load a TFLite interpreter with the tensor flow model.
  FaceDetectorGlobal detector(readFile(MODEL_PATH));

  ScopeTimer rt("test");
  auto faces = detector.detectFaces(img, {});

  EXPECT_EQ(1, faces.size());
}

}  // namespace c8
