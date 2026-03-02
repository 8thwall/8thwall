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
    "//reality/quality/datasets:benchmark-dataset",
    "@com_google_benchmark//:benchmark",
  };
}
cc_end(0x3c62e0c8);

#include "c8/pixels/pixel-transforms.h"

#include "c8/stats/scope-timer.h"
#include "reality/quality/datasets/benchmark-dataset.h"

#include <benchmark/benchmark.h>

using namespace c8;

namespace c8 {

class Benchmark480x640 : public benchmark::Fixture {
public:
  Benchmark480x640()
      : numImages(BenchmarkDataset::size(BenchmarkName::SIMPLE10_480x640)),
        index(0) {
    // Preload all of the benchmark images.
    images.reserve(numImages);
    for (int i = 0; i < numImages; ++i) {
      images.emplace_back(BenchmarkDataset::loadRGBA(BenchmarkName::SIMPLE10_480x640, i));
    }
  }

  // Get the next benchmark image.
  RGBA8888PlanePixelBuffer &nextImage() {
    auto &image = images[index];
    index = (index + 1) % numImages;
    return image;
  }

protected:
  const int numImages;
  Vector<RGBA8888PlanePixelBuffer> images;
  int index;
};

}  // namespace c8

namespace {
void splitPixelsOriginal(
  const ConstFourChannelPixels &src,
  OneChannelPixels *c1,
  OneChannelPixels *c2,
  OneChannelPixels *c3,
  OneChannelPixels *c4) {
  ScopeTimer t("split-four-channels-to-planes");
  int cn[4];
  int dStride[4];
  uint8_t *dStart[4];
  int n = 0;
  if (c1 != nullptr) {
    cn[n] = 0;
    dStride[n] = c1->rowBytes();
    dStart[n] = c1->pixels();
    ++n;
  }
  if (c2 != nullptr) {
    cn[n] = 1;
    dStride[n] = c2->rowBytes();
    dStart[n] = c2->pixels();
    ++n;
  }
  if (c3 != nullptr) {
    cn[n] = 2;
    dStride[n] = c3->rowBytes();
    dStart[n] = c3->pixels();
    ++n;
  }
  if (c4 != nullptr) {
    cn[n] = 4;
    dStride[n] = c4->rowBytes();
    dStart[n] = c4->pixels();
    ++n;
  }

  if (n == 0) {
    return;
  }

  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcStride = src.rowBytes();
  const uint8_t *srcStart = src.pixels();

  if (n == 1) {
    const int d0Stride = dStride[0];
    uint8_t *d0Start = dStart[0];
    int cn0 = cn[0];

    for (int r = 0; r < srcHeight; ++r) {
      const uint8_t *srcEnd = srcStart + 4 * srcWidth;
      const uint8_t *s = srcStart;
      uint8_t *d0 = d0Start;
      while (s < srcEnd) {
        *d0 = s[cn0];
        ++d0;
        s += 4;
      }
      d0Start += d0Stride;
      srcStart += srcStride;
    }
  }

  if (n == 2) {
    const int d0Stride = dStride[0];
    const int d1Stride = dStride[1];
    uint8_t *d0Start = dStart[0];
    uint8_t *d1Start = dStart[1];
    int cn0 = cn[0];
    int cn1 = cn[1];

    for (int r = 0; r < srcHeight; ++r) {
      const uint8_t *srcEnd = srcStart + 4 * srcWidth;
      const uint8_t *s = srcStart;
      uint8_t *d0 = d0Start;
      uint8_t *d1 = d1Start;
      while (s < srcEnd) {
        *d0 = s[cn0];
        *d1 = s[cn1];
        ++d0;
        ++d1;
        s += 4;
      }
      d0Start += d0Stride;
      d1Start += d1Stride;
      srcStart += srcStride;
    }
  }

  if (n == 3) {
    const int d0Stride = dStride[0];
    const int d1Stride = dStride[1];
    const int d2Stride = dStride[2];
    uint8_t *d0Start = dStart[0];
    uint8_t *d1Start = dStart[1];
    uint8_t *d2Start = dStart[2];
    int cn0 = cn[0];
    int cn1 = cn[1];
    int cn2 = cn[2];

    for (int r = 0; r < srcHeight; ++r) {
      const uint8_t *srcEnd = srcStart + 4 * srcWidth;
      const uint8_t *s = srcStart;
      uint8_t *d0 = d0Start;
      uint8_t *d1 = d1Start;
      uint8_t *d2 = d2Start;
      while (s < srcEnd) {
        *d0 = s[cn0];
        *d1 = s[cn1];
        *d2 = s[cn2];
        ++d0;
        ++d1;
        ++d2;
        s += 4;
      }
      d0Start += d0Stride;
      d1Start += d1Stride;
      d2Start += d2Stride;
      srcStart += srcStride;
    }
  }

  if (n == 4) {
    const int d0Stride = dStride[0];
    const int d1Stride = dStride[1];
    const int d2Stride = dStride[2];
    const int d3Stride = dStride[3];
    uint8_t *d0Start = dStart[0];
    uint8_t *d1Start = dStart[1];
    uint8_t *d2Start = dStart[2];
    uint8_t *d3Start = dStart[3];
    int cn0 = cn[0];
    int cn1 = cn[1];
    int cn2 = cn[2];
    int cn3 = cn[3];

    for (int r = 0; r < srcHeight; ++r) {
      const uint8_t *srcEnd = srcStart + 4 * srcWidth;
      const uint8_t *s = srcStart;
      uint8_t *d0 = d0Start;
      uint8_t *d1 = d1Start;
      uint8_t *d2 = d2Start;
      uint8_t *d3 = d3Start;
      while (s < srcEnd) {
        *d0 = s[cn0];
        *d1 = s[cn1];
        *d2 = s[cn2];
        *d3 = s[cn3];
        ++d0;
        ++d1;
        ++d2;
        ++d3;
        s += 4;
      }
      d0Start += d0Stride;
      d1Start += d1Stride;
      d2Start += d2Stride;
      d3Start += d3Stride;
      srcStart += srcStride;
    }
  }
}

}  // namespace

BENCHMARK_F(Benchmark480x640, splitPixels1ChannelOriginal)(benchmark::State &state) {
  YPlanePixelBuffer c1(640, 480);
  YPlanePixelBuffer c2(640, 480);
  YPlanePixelBuffer c3(640, 480);

  auto pc1 = c1.pixels();
  auto pc2 = c2.pixels();
  auto pc3 = c3.pixels();

  for (auto _ : state) {
    const auto &image = nextImage();
    splitPixelsOriginal(image.pixels(), &pc1, nullptr, nullptr, nullptr);
  }
}

BENCHMARK_F(Benchmark480x640, splitPixels1ChannelHead)(benchmark::State &state) {
  YPlanePixelBuffer c1(640, 480);
  YPlanePixelBuffer c2(640, 480);
  YPlanePixelBuffer c3(640, 480);

  auto pc1 = c1.pixels();
  auto pc2 = c2.pixels();
  auto pc3 = c3.pixels();

  for (auto _ : state) {
    const auto &image = nextImage();
    splitPixels(image.pixels(), &pc1, nullptr, nullptr, nullptr);
  }
}

BENCHMARK_F(Benchmark480x640, splitPixels3ChannelOriginal)(benchmark::State &state) {
  YPlanePixelBuffer c1(640, 480);
  YPlanePixelBuffer c2(640, 480);
  YPlanePixelBuffer c3(640, 480);

  auto pc1 = c1.pixels();
  auto pc2 = c2.pixels();
  auto pc3 = c3.pixels();

  for (auto _ : state) {
    const auto &image = nextImage();
    splitPixelsOriginal(image.pixels(), &pc1, &pc2, &pc3, nullptr);
  }
}

BENCHMARK_F(Benchmark480x640, splitPixels3ChannelHead)(benchmark::State &state) {
  YPlanePixelBuffer c1(640, 480);
  YPlanePixelBuffer c2(640, 480);
  YPlanePixelBuffer c3(640, 480);

  auto pc1 = c1.pixels();
  auto pc2 = c2.pixels();
  auto pc3 = c3.pixels();

  for (auto _ : state) {
    const auto &image = nextImage();
    splitPixels(image.pixels(), &pc1, &pc2, &pc3, nullptr);
  }
}
