// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules.h"

cc_library {
  hdrs = {
    "capnp-messages.h"
  };
  deps = {
    "//bzl/inliner:rules",
    "@capnproto//:capnp-lib",
  };
  visibility = {
    "//visibility:public",
  };
}

#include "c8/io/capnp-messages.h"

using namespace c8;
