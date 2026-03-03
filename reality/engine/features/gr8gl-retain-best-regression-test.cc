// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "@com_google_googletest//:gtest_main",
    ":gr8gl",
    ":gr8gl-slow",
    "//reality/quality/datasets:benchmark-dataset",
    "//third_party/cvlite/features2d:fast",
    "//third_party/cvlite/features2d:keypoint",
  };
}
cc_end(0x3bda69ff);

#include "c8/c8-log.h"
#include "reality/engine/features/gr8gl-slow.h"
#include "reality/engine/features/gr8gl.h"
#include "reality/engine/features/keypoint-queue.h"
#include "reality/quality/datasets/benchmark-dataset.h"
#include "third_party/cvlite/features2d/fast.h"
#include "third_party/cvlite/features2d/keypoint.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ContainerEq;

namespace c8 {

class Gr8GlRetainBestRegressionTest : public ::testing::Test {
public:
  static constexpr int FAST_THRESHOLD = 5;
  Gr8GlRetainBestRegressionTest()
      : numImages(BenchmarkDataset::size(BenchmarkName::SIMPLE10_480x640)) {
    ScopeTimer rt("test");
    cols.reserve(numImages);
    rows.reserve(numImages);
    for (int i = 0; i < numImages; ++i) {
      auto image = BenchmarkDataset::loadY(BenchmarkName::SIMPLE10_480x640, i);
      auto yPix = image.pixels();
      auto mat = c8cv::Mat(yPix.rows(), yPix.cols(), CV_8U, yPix.pixels(), yPix.rowBytes());
      cols.emplace_back(yPix.cols());
      rows.emplace_back(yPix.rows());
      c8cv::Mat descriptors;
      keyPointsArray.emplace_back();

      c8cv::Ptr<c8cv::FastFeatureDetector> fd =
        c8cv::FastFeatureDetector::create(FAST_THRESHOLD, true);
      Vector<c8cv::KeyPoint> cvpts;
      fd->detect(mat, cvpts);
      for (auto pt : cvpts) {
        ImagePointLocation l;
        l.pt.x = pt.pt.x;
        l.pt.y = pt.pt.y;
        l.scale = pt.octave;
        l.angle = pt.angle;
        l.response = pt.response;
        keyPointsArray.back().push_back(l);
      }
    }
  }

protected:
  const int numImages;
  std::vector<std::vector<ImagePointLocation>> keyPointsArray;
  std::vector<int> cols;
  std::vector<int> rows;
};

struct KeyPointComp {
  inline bool operator()(const ImagePointLocation &kp1, const ImagePointLocation &kp2) const {
    if (kp1.response != kp2.response) {
      return kp1.response > kp2.response;
    }
    return kp1.pt.x != kp2.pt.x ? kp1.pt.x < kp2.pt.x : kp1.pt.y < kp2.pt.y;
  }
};

TEST_F(Gr8GlRetainBestRegressionTest, TestOriginalAndNewVersionsAreSame) {
  for (int i = 0; i < numImages; ++i) {
    std::vector<ImagePointLocation> slow = keyPointsArray[i];
    std::vector<ImagePointLocation> fast = keyPointsArray[i];
    KeyPointComp comp;
    std::sort(slow.begin(), slow.end(), comp);
    std::sort(fast.begin(), fast.end(), comp);
    auto col = cols[i];
    auto row = rows[i];
    Gr8GlSlow::retainBest(slow, slow.size() / (i + 1), col);
    Gr8Gl::retainBest(fast, fast.size() / (i + 1), col, row, 0, true);

    EXPECT_EQ(slow.size(), fast.size());
    for (int i = 0; i < slow.size(); i++) {
      ASSERT_EQ(slow[i], fast[i]) << "Slow " << slow[i].pt.x << " " << slow[i].pt.y << " "
                                  << slow[i].response << ". Fast " << fast[i].pt.x << " "
                                  << fast[i].pt.y << " " << fast[i].response;
    }
  }
}

}  // namespace c8
