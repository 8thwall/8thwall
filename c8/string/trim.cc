// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//visibility:public",
  };
  hdrs = {"trim.h"},
  deps = {
    "//c8:string",
    "//c8:string-view",
  };
}
cc_end(0xe94317fb);

#include <regex>

#include "c8/string/trim.h"

namespace c8 {

namespace {
const std::regex TRAILING_LEADING_SPACE_REGEX(R"(^\s+|\s+$)");
}  // namespace

String strTrim(StringView str) {
  return std::regex_replace(str.begin(), TRAILING_LEADING_SPACE_REGEX, "");
}

}  // namespace c8
