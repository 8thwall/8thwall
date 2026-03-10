// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// To run this benchmark, use the following command:
/*
     bazel test reality/engine/features/gr8-benchmark-test --cache_test_results=false \
       --test_output=all
*/
// You can run the test directly with --benchmark_repetitions=N and
// --benchmark_report_aggregates_only=true flags for more accurate results.

#include <random>

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":gr8cpu",
    "//reality/quality/datasets:benchmark-dataset",
    "@com_google_benchmark//:benchmark",
  };
}
cc_end(0xf7756424);

#include <benchmark/benchmark.h>

#include "reality/engine/features/gr8cpu.h"
#include "c8/stats/scope-timer.h"
#include "reality/quality/datasets/benchmark-dataset.h"

namespace c8 {

class Benchmark480x640 : public benchmark::Fixture {
public:
  Benchmark480x640()
      : numImages(BenchmarkDataset::size(BenchmarkName::SIMPLE10_480x640)), index(0) {
    // Preload all of the benchmark images.
    images.reserve(numImages);
    for (int i = 0; i < numImages; ++i) {
      images.emplace_back(BenchmarkDataset::loadY(BenchmarkName::SIMPLE10_480x640, i));
      auto yPix = images.back().pixels();
    }
  }

  // Get the next benchmark image.
  YPlanePixels nextImage() {
    auto image = images[index].pixels();
    index = (index + 1) % numImages;
    return image;
  }

protected:
  const int numImages;
  Vector<YPlanePixelBuffer> images;
  int index;
};

BENCHMARK_F(Benchmark480x640, Gr8_CPU)(benchmark::State &state) {
  ScopeTimer rt("test");
  // These mirrors the parameters that are in 8th Wall XR.
  auto extractor = Gr8Cpu::create(500, 1.44f, 4, 31, 0, 2, Gr8CpuInterface::FAST_SCORE, 5);

  for (auto _ : state) {
    auto res = extractor.detectAndCompute(nextImage());
  }
}

// TODO(christoph): Make the following benchmark use the future OpenGL version.
BENCHMARK_F(Benchmark480x640, Gr8_OpenGL)(benchmark::State &state) {
  ScopeTimer rt("test");

  // These mirrors the parameters that are in 8th Wall XR.
  auto extractor = Gr8Cpu::create(500, 1.44f, 4, 31, 0, 2, Gr8CpuInterface::FAST_SCORE, 5);

  for (auto _ : state) {
    auto res = extractor.detectAndCompute(nextImage());
  }
}

}  // namespace c8
