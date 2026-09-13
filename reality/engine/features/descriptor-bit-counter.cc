// Copyright (c) 2024 Niantic, Inc.
// Original Author: Haomin Zhu (hzhu@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "descriptor-bit-counter.h",
  };
  deps = {
    "//c8:exceptions",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xd958b762);

#include "descriptor-bit-counter.h"
