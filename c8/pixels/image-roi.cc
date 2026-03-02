// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "image-roi.h",
  };
  deps = {
    "//c8:hmatrix",
    "//c8/geometry:parameterized-geometry",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x511b5c12);

#include "c8/pixels/image-roi.h"

namespace c8 {}  // namespace c8
