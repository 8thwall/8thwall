// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"containers.h"};
  deps = {
    "//c8:vector",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x39dd1344);

#include "c8/string/containers.h"

namespace c8 {}  // namespace c8
