// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Nathan Waters (nathan@8thwall.com)
// Common structs used for meshes

#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "mesh-types.h",
  };
  deps = {
    "//c8:color",
    "//c8:hpoint",
    "//c8:vector",
    "//c8:hvector",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x98161979);

#include "c8/geometry/mesh-types.h"

namespace c8 {
// empty
}
