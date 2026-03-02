// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "test-image.h",
  };
  deps = {
    ":pixel-buffer",
    ":pixel-transforms",
    ":pixels",
    "//c8/time:now",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xc0771a27);

#include "c8/pixels/test-image.h"

#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/time/now.h"

namespace {

constexpr int64_t next(int64_t &s) {
  s = (s * 4103861 + 4103881) % 4103887;
  return s;
}

}  // namespace

namespace c8 {

void runTestImageBenchmark(int r, int c, std::array<float, 4> *meanPixelValues, int64_t *micros) {
  auto start = nowMicros();
  RGBA8888PlanePixelBuffer im(r, c);
  fillTestImage(im.pixels());
  *meanPixelValues = meanPixelValue(im.pixels());
  *micros = nowMicros() - start;
}

void fillTestImage(RGBA8888PlanePixels tp) { fillTestImage(tp, 0); }

void fillTestImage(RGBA8888PlanePixels tp, int64_t seed) {
  int64_t s = seed;
  for (int r = 0; r < tp.rows(); ++r) {
    for (int c = 0; c < tp.cols(); ++c) {
      auto *p = tp.pixels() + r * tp.rowBytes() + c * 4;
      p[0] = next(s) % 255;
      p[1] = next(s) % 255;
      p[2] = next(s) % 255;
      p[3] = 255;
    }
  }
}

bool eqImg(RGBA8888PlanePixels a, RGBA8888PlanePixels b, int thresh) {
  bool passed = true;
  for (int r = 0; r < a.rows(); ++r) {
    for (int c = 0; c < a.cols(); ++c) {
      auto *pa = a.pixels() + r * a.rowBytes() + c * 4;
      auto *pb = b.pixels() + r * b.rowBytes() + c * 4;
      passed &= std::abs(pa[0] - pb[0]) <= thresh;
      passed &= std::abs(pa[1] - pb[1]) <= thresh;
      passed &= std::abs(pa[2] - pb[2]) <= thresh;
    }
  }
  return passed;
}

}  // namespace c8
