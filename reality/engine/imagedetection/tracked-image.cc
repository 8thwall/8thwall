// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"tracked-image.h"};
  deps = {
    "//c8:hmatrix",
    "//c8/pixels:image-roi",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x84004842);

namespace c8 {}
