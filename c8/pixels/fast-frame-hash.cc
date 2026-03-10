// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "fast-frame-hash.h",
  };
  deps = {
    ":pixels",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x76022676);

#include "c8/pixels/fast-frame-hash.h"

#include <cmath>

namespace c8 {

void FastFrameHash::fill(ConstRGBA8888PlanePixels p) {
  float cstep = p.cols() / 11;
  float rstep = p.rows() / 11;
  float r = rstep;
  int idx = 0;
  for (int i = 0; i < 10; ++i) {
    float c = cstep;
    int y = static_cast<int>(std::round(r)) * p.rowBytes();
    for (int j = 0; j < 10; ++j) {
      int x = static_cast<int>(std::round(c)) << 2;
      // clang-format off
      pixelValues_[idx++] =
        p.pixels()[y + x - 8]
        ^ p.pixels()[y + x - 4]
        ^ p.pixels()[y + x]
        ^ p.pixels()[y + x + 4]
        ^ p.pixels()[y + x + 8];
      // clang-format on
      c += cstep;
    }
    r += rstep;
  }
}

}  // namespace c8
