// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "capnp-messages.h"
  };
  deps = {
    "@capnproto//:capnp-lib",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xd1c291cb);

#include "c8/io/capnp-messages.h"

using namespace c8;
