// Copyright (c) 2022 Niantic Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"contains.h"};
  deps = {
    "//c8:string-view",
    "//c8:string",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x5365820d);

#include "c8/string/contains.h"

namespace c8 {}  // namespace c8
