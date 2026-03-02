// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "hash.h",
  };
  visibility = {
    "//visibility:public",
  };
  deps = {
    "//third_party/smhasher:murmurhash3",
  };
}
cc_end(0x178e76f9);

#include "c8/hash.h"

namespace c8 {
uint32_t murmurHash32(const char *v, int sizeOfV) {
  uint32_t res = 0;
  MurmurHash3_x86_32(v, sizeOfV, 123456789, &res);
  return res;
}
}  // namespace c8
