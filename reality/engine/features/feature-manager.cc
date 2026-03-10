// Copyright (c) 2024 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "feature-manager.h",
  };
  deps = {
    "//c8:vector",
    "//c8:exceptions",
    "//reality/engine/features:image-descriptor",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x6cb435bd);

#include "reality/engine/features/feature-manager.h"

namespace c8 {}
