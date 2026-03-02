// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
/*
#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":image-point",
    "//c8:exceptions",
    "//reality/quality/datasets:benchmark-dataset",
    "//third_party/cvlite/features2d:fast",
    "//third_party/cvlite/features2d:keypoint",
    "@com_google_benchmark//:benchmark",
  };
}
cc_end(0x5b2efd64);
*/

#include <benchmark/benchmark.h>

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "reality/engine/features/image-point.h"
#include "reality/quality/datasets/benchmark-dataset.h"
#include "third_party/cvlite/features2d/fast.h"
#include "third_party/cvlite/features2d/keypoint.h"

namespace c8 {

class Gr8GlExtractFeaturesBenchmarkTest : public benchmark::Fixture {
public:
  static constexpr int FAST_THRESHOLD = 5;
  Gr8GlExtractFeaturesBenchmarkTest()
      : numImages(BenchmarkDataset::size(BenchmarkName::SIMPLE10_480x640)) {
    C8Log("[Gr8GlExtractFeaturesBenchmarkTest] %s", "Initializing benchmark");
    ScopeTimer rt("test");
    cols.reserve(numImages);
    rows.reserve(numImages);
    for (int i = 0; i < numImages; ++i) {
      C8Log("Loading image [%d]", i);
      auto image = BenchmarkDataset::loadY(BenchmarkName::SIMPLE10_480x640, i);
      C8Log("-- Loaded image [%d]", i);
      auto yPix = image.pixels();
      auto mat = c8cv::Mat(yPix.rows(), yPix.cols(), CV_8U, yPix.pixels(), yPix.rowBytes());
      cols.emplace_back(yPix.cols());
      rows.emplace_back(yPix.rows());
      c8cv::Mat descriptors;

      c8cv::Ptr<c8cv::FastFeatureDetector> fd =
        c8cv::FastFeatureDetector::create(FAST_THRESHOLD, true);
      Vector<c8cv::KeyPoint> cvpts;
      fd->detect(mat, cvpts);
      featIms.emplace_back(yPix.rows(), yPix.cols());
      auto featVals = featIms.back().pixels();
      std::memset(featVals.pixels(), 0, featVals.rows() * featVals.rowBytes());
      for (auto pt : cvpts) {
        int r = static_cast<int>(pt.pt.y);
        int c = static_cast<int>(pt.pt.x);
        featVals.pixels()[r * featVals.rowBytes() + c] = pt.response;
      }
      numPts.push_back(cvpts.size());
    }
  }

protected:
  const int numImages;
  std::vector<YPlanePixelBuffer> featIms;
  std::vector<int> numPts;
  std::vector<int> cols;
  std::vector<int> rows;
};

BENCHMARK_F(Gr8GlExtractFeaturesBenchmarkTest, extractPointsIndirect)(benchmark::State &state) {
  int i = 0;
  std::vector<ImagePointLocation> keypoints;
  for (auto _ : state) {
    keypoints.clear();
    keypoints.reserve(numPts[i]);
    auto im = featIms[i].pixels();

    std::array<const uint8_t *, 30000> ptidxs;  // 30000 ~= 480 * 64, 1/10 of the image rows.

    int r = im.rows();
    int c = im.cols();

    int nptidxs = 0;
    const uint8_t *rowStart = im.pixels();
    for (int i = 0; i < r; ++i) {
      const uint8_t *e = rowStart + c;
      for (const uint8_t *p = rowStart; p < e; ++p) {
        if (*p) {
          ptidxs[nptidxs++] = p;
        }
      }
      if (c > (ptidxs.size() - nptidxs) || i == r - 1) {
        for (int idx = 0; idx < nptidxs; ++idx) {
          const uint8_t *pt = ptidxs[idx];
          int off = pt - im.pixels();
          int y = off / im.rowBytes();
          int x = off % im.rowBytes();
          keypoints.emplace_back(x, y, 0, 7.0f, -1, -1, *pt, -1);
        }
        nptidxs = 0;
      }
      rowStart += im.rowBytes();
    }
    if (keypoints.size() != numPts[i]) {
      C8Log("[extractPointsIndirect] Expect %d keypoints but got %d", keypoints.size(), numPts[i]);
      C8_THROW("extracted wrong number of keypoints.");
    }
    i = (i + 1) % numImages;
  }
}

BENCHMARK_F(Gr8GlExtractFeaturesBenchmarkTest, extractPointsDirect)(benchmark::State &state) {
  int i = 0;
  std::vector<ImagePointLocation> keypoints;
  for (auto _ : state) {
    keypoints.clear();
    keypoints.reserve(numPts[i]);
    auto im = featIms[i].pixels();

    int r = im.rows();
    int c = im.cols();

    const uint8_t *rowStart = im.pixels();
    for (int i = 0; i < r; ++i) {
      const uint8_t *e = rowStart + c;
      for (const uint8_t *p = rowStart; p < e; ++p) {
        if (*p) {
          keypoints.emplace_back(c, r, 0, 7.0f, -1, -1, *p, -1);
        }
      }
      rowStart += im.rowBytes();
    }
    if (keypoints.size() != numPts[i]) {
      C8Log("[extractPointsDirect] Expect %d keypoints but got %d", keypoints.size(), numPts[i]);
      C8_THROW("extracted wrong number of keypoints.");
    }
    i = (i + 1) % numImages;
  }
}

}  // namespace c8
