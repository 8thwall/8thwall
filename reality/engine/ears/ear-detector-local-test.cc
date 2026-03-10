// Copyright (c) 2023 Niantic Labs
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":ear-detector-local",
    ":ear-types",
    "//c8:string",
    "//c8/camera:device-infos",
    "//c8/geometry:intrinsics",
    "//c8/io:file-io",
    "//c8/io:image-io",
    "//c8/io:model-3d-io",
    "//c8/pixels:pixel-transforms",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//reality/engine/deepnets/testdata:ear-left",
    "//reality/engine/deepnets/testdata:ear-right",
    "//reality/engine/ears/data:earmodel",
  };
  linkstatic = 1;
}
cc_end(0xcac87b20);

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>

#include "c8/camera/device-infos.h"
#include "c8/geometry/intrinsics.h"
#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/io/model-3d-io.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/stats/scope-timer.h"
#include "c8/string.h"
#include "gtest/gtest.h"
#include "reality/engine/ears/ear-detector-local.h"
#include "reality/engine/ears/ear-types.h"

namespace c8 {

class EarDetectorLocalTest : public ::testing::Test {};

static constexpr char EAR_LEFT_IMAGE_PATH[] = "reality/engine/deepnets/testdata/ear_left.jpg";
static constexpr char EAR_RIGHT_IMAGE_PATH[] = "reality/engine/deepnets/testdata/ear_right.jpg";

static constexpr char EAR_LANDMARK_MODEL_PATH[] = "reality/engine/ears/data/ear_model_v2.tflite";

static constexpr bool WRITE_TO_IMAGE = false;
static constexpr char EAR_DETECTOR_INPUT_IMAGE_PATH[] = "/tmp/ear_input.jpg";
static constexpr char EAR_DETECTOR_OUTPUT_IMAGE_PATH[] = "/tmp/ear_result.jpg";

TEST_F(EarDetectorLocalTest, TestAnaylyze) {
  auto k = Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_12);

  // Read in an image. The test image has w112xh160 dimensions.
  auto lim = readImageToRGBA(EAR_LEFT_IMAGE_PATH);
  auto lpix = lim.pixels();
  auto rim = readImageToRGBA(EAR_RIGHT_IMAGE_PATH);
  auto rpix = rim.pixels();

  if (WRITE_TO_IMAGE) {
    // replicate data preparation step in EarDetectorLocal::analyzeEars()
    std::unique_ptr<float> fImg(
      new float[3 * EAR_LANDMARK_DETECTION_INPUT_WIDTH * 2 * EAR_LANDMARK_DETECTION_INPUT_HEIGHT]);
    float *fImgData = fImg.get();
    toLetterboxRGBFloat0To1(
      lpix, EAR_LANDMARK_DETECTION_INPUT_WIDTH, EAR_LANDMARK_DETECTION_INPUT_HEIGHT, fImgData);

    fImgData += 3 * EAR_LANDMARK_DETECTION_INPUT_WIDTH * EAR_LANDMARK_DETECTION_INPUT_HEIGHT;
    toLetterboxRGBFloat0To1FlipX(
      rpix, EAR_LANDMARK_DETECTION_INPUT_WIDTH, EAR_LANDMARK_DETECTION_INPUT_HEIGHT, fImgData);

    // from float to uint8_t, write image to EAR_DETECTOR_INPUT_IMAGE_PATH
    auto inputPixBuffer = RGBA8888PlanePixelBuffer(
      2 * EAR_LANDMARK_DETECTION_INPUT_HEIGHT, EAR_LANDMARK_DETECTION_INPUT_WIDTH);
    uint8_t *u8Ptr = inputPixBuffer.pixels().pixels();
    fImgData = fImg.get();
    for (size_t k = 0;
         k < 2 * EAR_LANDMARK_DETECTION_INPUT_HEIGHT * EAR_LANDMARK_DETECTION_INPUT_WIDTH;
         ++k) {
      u8Ptr[0] = static_cast<uint8_t>(fImgData[0] * 255);
      u8Ptr[1] = static_cast<uint8_t>(fImgData[1] * 255);
      u8Ptr[2] = static_cast<uint8_t>(fImgData[2] * 255);
      u8Ptr[3] = 255;
      u8Ptr += 4;
      fImgData += 3;
    }

    writePixelsToJpg(inputPixBuffer.pixels(), EAR_DETECTOR_INPUT_IMAGE_PATH);
  }

  // Take the test images with well centered ears.
  RenderedSubImage lImg{
    {0, 0, 112, 160},
    lpix,
    {
      ImageRoi::Source::EAR_LEFT,
      0,
      "",
      HMatrixGen::i(),
    }};

  // Right ear will be mirrored, here we re-use the same pixels as the left ear.
  RenderedSubImage rImg{
    {0, 0, 112, 160},
    rpix,
    {
      ImageRoi::Source::EAR_RIGHT,
      0,
      "",
      HMatrixGen::i(),
    }};

  // Load a TFLite interpreter with the tensor flow model.
  EarDetectorLocal detector(readFile(EAR_LANDMARK_MODEL_PATH));

  ScopeTimer rt("test-ears");
  auto ears = detector.analyzeEars(lImg, rImg, k);

  EXPECT_EQ(2, ears.size());

  // locations for point pairs should be x-mirrored
  DetectedPoints &left = ears[0];
  DetectedPoints &right = ears[1];
  for (int i = 0; i < EAR_LANDMARK_DETECTION_NUM_PER_EAR; ++i) {
    EXPECT_NEAR(left.points[i].x() + right.points[i].x(), 1.0f, 0.00005);
    EXPECT_NEAR(left.points[i].y(), right.points[i].y(), 0.01f);
    // all ear points should be visible
    EXPECT_TRUE(left.points[i].z() > EAR_LANDMARK_DETECTION_VISIBILITY_THRESHOLD);
    EXPECT_TRUE(right.points[i].z() > EAR_LANDMARK_DETECTION_VISIBILITY_THRESHOLD);
  }
}

}  // namespace c8
