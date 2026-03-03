// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"target-point.h"};
  deps = {
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:quaternion",
    "//c8:set",
    "//c8:vector",
    "//c8/geometry:egomotion",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//c8/pixels:image-roi",
    "//reality/engine/features:image-descriptor",
    "//reality/engine/features:feature-manager",
    "//reality/engine:xr-id",
  };
}
cc_end(0xcc754602);

#include "reality/engine/imagedetection/target-point.h"

namespace c8 {}
