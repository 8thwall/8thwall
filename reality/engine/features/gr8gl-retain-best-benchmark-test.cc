// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "@com_google_benchmark//:benchmark",
    ":gr8gl",
    ":gr8gl-slow",
    "//reality/quality/datasets:benchmark-dataset",
    "//third_party/cvlite/features2d:fast",
    "//third_party/cvlite/features2d:keypoint",
  };
}
cc_end(0xa4b27889);

#include "reality/engine/features/gr8gl-slow.h"
#include "reality/engine/features/gr8gl.h"
#include "reality/quality/datasets/benchmark-dataset.h"
#include "third_party/cvlite/features2d/keypoint.h"
#include "third_party/cvlite/features2d/fast.h"
#include <benchmark/benchmark.h>

namespace c8 {

class Gr8GlRetainBestBenchmarkTest : public benchmark::Fixture {
public:
  static constexpr int FAST_THRESHOLD = 5;
  Gr8GlRetainBestBenchmarkTest()
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

      c8cv::Ptr<c8cv::FastFeatureDetector> fd = c8cv::FastFeatureDetector::create(FAST_THRESHOLD, true);
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

BENCHMARK_F(Gr8GlRetainBestBenchmarkTest, retainBestSlow)(benchmark::State &state) {
  int i = 0;
  std::vector<ImagePointLocation> points;
  for (auto _ : state) {
    points = keyPointsArray[i];
    auto col = cols.at(i);
    Gr8GlSlow::retainBest(points, points.size() / 2, col);
    i = (i + 1) % numImages;
  }
}

BENCHMARK_F(Gr8GlRetainBestBenchmarkTest, retainBestHead)(benchmark::State &state) {
  int i = 0;
  std::vector<ImagePointLocation> points;

  for (auto _ : state) {
    points = keyPointsArray[i];
    auto col = cols[i];
    auto row = rows[i];
    Gr8Gl::retainBest(points, points.size() / 2, col, row, 0, true);
    i = (i + 1) % numImages;

  }
}

}  // namespace c8
