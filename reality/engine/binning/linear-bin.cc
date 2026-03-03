// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"linear-bin.h"};
  visibility = {
    "//visibility:public",
  };
  deps = {
    "//c8:hvector",
    "//c8:vector",
  };
}
cc_end(0x1df9984f);

#include "reality/engine/binning/linear-bin.h"

namespace c8 {}
