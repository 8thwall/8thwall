// Copyright (c) 2025 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "detection-config.h",
  };
  deps = {
    "//reality/engine/features:feature-manager",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x363838f8);

#include "reality/engine/features/detection-config.h"

namespace c8 {}
