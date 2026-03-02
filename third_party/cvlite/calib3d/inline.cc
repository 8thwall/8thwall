// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
#include "bzl/inliner/rules.h"

cc_library {
  hdrs = {
    "inline.h",
  };
  deps = {
    "//bzl/inliner:rules",
    "//third_party/cvlite/core",
  };
  copts = {
    "-D__OPENCV_BUILD",
  };
}

#include "third_party/cvlite/calib3d/inline.h"

namespace c8 {

}
