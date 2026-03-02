// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//visibility:public",
  };
  hdrs = {"uniform.h"};
  deps = {
    "//c8:string",
    "//c8:vector",
  };
}
cc_end(0xe1f21169);

namespace c8 {}  // namespace c8
