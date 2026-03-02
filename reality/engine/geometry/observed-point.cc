// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)
//

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "observed-point.h",
  };
  deps = {
    "//c8:hpoint",
    "//c8/string:format",
    "//c8:string",
  };
  copts = {
    "-Wno-unused-private-field",
  };
}
cc_end(0x71c239c2);
