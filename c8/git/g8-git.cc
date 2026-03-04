// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "g8-git.h",
  };
  visibility = {
    "//visibility:public",
  };
  deps = {
    ":api-blobs",
    ":api-diff",
    ":api-files",
    ":api-repository",
    ":api-inspect",
    "//c8/git/api-changeset:api-changeset",
    "//c8/git/api-client:api-client",
  };
}
cc_end(0xd9ae4568);

namespace c8 {

// NOTE(pawel) all api functions are defined in api-*.cc files
// this file links to those individual compilation units

}  // namespace c8
