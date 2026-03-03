// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"detection-image-local-matcher.h"};
  deps = {
    ":target-point",
    "//c8:hpoint",
    "//c8:map",
    "//c8:vector",
    "//c8/stats:scope-timer",
    "//reality/engine/binning:linear-bin",
    "//reality/engine/features:frame-point",
    "//reality/engine/features:image-descriptor",
  };
}
cc_end(0x112b32b5);

#include "reality/engine/imagedetection/detection-image-local-matcher.h"

namespace c8 {}
