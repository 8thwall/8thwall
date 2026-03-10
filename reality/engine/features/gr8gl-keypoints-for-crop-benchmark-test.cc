// Copyright (c) 2022 Niantic Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "@com_google_benchmark//:benchmark",
    ":gr8gl",
    "//reality/quality/datasets:benchmark-dataset",
  };
}
cc_end(0x71ad0134);

#include "reality/engine/features/gr8gl.h"
#include "reality/quality/datasets/benchmark-dataset.h"
#include <benchmark/benchmark.h>

namespace c8 {

// These methods are from gr8gl.cc. We extern them so they are not exposed in gr8gl.h
extern void keypointsForCrop(
  ConstRGBA8888PlanePixels s,
  uint8_t scale,
  int8_t roi,
  int edgeThreshold,
  Vector<ImagePointLocation> &keypoints);
extern void keypointsForCropJs(
  ConstRGBA8888PlanePixels s,
  uint8_t scale,
  int8_t roi,
  int edgeThreshold,
  Vector<ImagePointLocation> &keypoints);

class Gr8glKeypointsForCropBenchmarkTest : public benchmark::Fixture {
public:
  static constexpr int FAST_THRESHOLD = 5;
  Gr8glKeypointsForCropBenchmarkTest()
      : numImages_(BenchmarkDataset::size(BenchmarkName::SIMPLE10_480x640)) {
    images_.reserve(numImages_);
    for (int i = 0; i < numImages_; ++i) {
      images_.push_back(BenchmarkDataset::loadRGBA(BenchmarkName::SIMPLE10_480x640, i));
    }
  }

protected:
  // fixed parameters that don't affect benchmark
  uint8_t scale = 2;
  int8_t roi = 0;
  int edgeThreshold = 4;

  // list of data that we can cycle through
  const int numImages_;
  Vector<RGBA8888PlanePixelBuffer> images_;
};

BENCHMARK_F(Gr8glKeypointsForCropBenchmarkTest, keypointsForCrop)(benchmark::State &state) {
  Vector<ImagePointLocation> outKps;
  int i = 0;
  for (auto _ : state) {
    outKps.clear();
    keypointsForCrop(images_[i].pixels(), scale, roi, edgeThreshold, outKps);
    i = (i + 1) % numImages_;
  }
}

BENCHMARK_F(Gr8glKeypointsForCropBenchmarkTest, keypointsForCropJs)(benchmark::State &state) {
  Vector<ImagePointLocation> outKps;
  int i = 0;
  for (auto _ : state) {
    outKps.clear();
    keypointsForCropJs(images_[i].pixels(), scale, roi, edgeThreshold, outKps);
    i = (i + 1) % numImages_;
  }
}

}  // namespace c8
