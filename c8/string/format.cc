// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "format.h",
  };
  deps = {
    "//c8:string",
    "//c8:vector",
  };
}
cc_end(0x96bc4013);

#include <algorithm>
#include <cctype>

#include "c8/string/format.h"

namespace c8 {

String toUpperCase(String s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
  return s;
}

String boolToString(bool b) { return b ? "true" : "false"; }

}  // namespace c8
