// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "split.h",
  };
  deps = {
    "//c8:string",
    "//c8:vector",

  };
}
cc_end(0x424d6951);

#include "c8/string/split.h"

namespace c8 {

Vector<String> split(const String &src, const String &del) {
  if (del.empty()) {
    return {src};
  }
  size_t pos = 0;
  String s = src;
  Vector<String> tokens;
  while ((pos = s.find(del)) != std::string::npos) {
    tokens.push_back(s.substr(0, pos));
    s.erase(0, pos + del.length());
  }
  tokens.push_back(s);
  return tokens;
}

}  // namespace c8
