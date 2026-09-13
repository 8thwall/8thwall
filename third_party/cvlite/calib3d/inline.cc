// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "inline.h",
  };
  deps = {
    "//third_party/cvlite/core",
  };
  copts = {
    "-D__OPENCV_BUILD",
  };
}
cc_end(0xb746d4c3);

#include "third_party/cvlite/calib3d/inline.h"

namespace c8 {

}
