// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"


cc_test {
  size = "small";
  deps = {
    ":opencv-image-descriptor",
    ":gr8",
    "//third_party/cvlite/core:core",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x25c861ee);

#include "reality/engine/features/opencv-image-descriptor.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "reality/engine/features/gr8.h"
#include "c8/stats/scope-timer.h"
#include "third_party/cvlite/core/core.hpp"

namespace c8 {

class OpencvImageDescriptorTest : public ::testing::Test {};

TEST_F(OpencvImageDescriptorTest, ExtractAndCreateDescriptor) {
  auto extractor = Gr8::create(500, 1.2f, 8, 31, 0, 2, Gr8::HARRIS_SCORE, 5);
  ScopeTimer rt("test");

  // Create a frames that is large enough for point detection to work. This
  // looks roughly like:
  // 0011
  // 1111
  // with a single salient corner point.
  c8cv::Mat frame(80, 80, CV_8UC1, 255);
  frame.rowRange(39, 40).colRange(39, 40) = 0;

  std::vector<c8cv::KeyPoint> keyPoints;
  c8cv::Mat descriptors;

  // Extract features from the test image.
  extractor.detectAndCompute(frame, keyPoints, descriptors);

  ASSERT_GE(keyPoints.size(), 1);
  ASSERT_GE(descriptors.rows, 1);
  ASSERT_EQ(32, descriptors.cols);

  const auto &firstDescriptor = descriptors.rowRange(0, 1);

  ImageDescriptor32 imageDescriptor = toImageDescriptor<32>(firstDescriptor);
  EXPECT_EQ(imageDescriptor.size(), 32);
  EXPECT_EQ(memcmp(imageDescriptor.data(), firstDescriptor.data, imageDescriptor.size()), 0);

  // If we modify the original descriptor, they should no longer be equal (not a shallow copy).
  firstDescriptor.colRange(0, 1) = 255;
  EXPECT_NE(memcmp(imageDescriptor.data(), firstDescriptor.data, imageDescriptor.size()), 0);
}

}  // namespace c8
