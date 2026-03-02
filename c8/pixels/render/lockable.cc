// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"lockable.h"};
  deps = {};
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x7aa278b6);

#include "c8/pixels/render/lockable.h"

namespace c8 {}  // namespace c8
