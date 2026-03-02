// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "c8-log-proto.h",
  };
  deps = {
    ":c8-log",
    ":string",
    "@capnproto//:capnp-lib",
  };
}
cc_end(0xb87c5504);

#include "c8/c8-log-proto.h"

#include <capnp/pretty-print.h>
#include <sstream>
#include "c8/c8-log.h"
#include "c8/string.h"

namespace c8 {

namespace {

void logAbbreviatedMessage(const char *raw) {
  std::istringstream stream(raw);
  std::stringstream out;
  for (std::string line; std::getline(stream, line);) {
    if (line.size() < 5) {
      C8Log("%s", line.c_str());
      continue;
    }
    std::string endString = line.substr(line.size() - 5, line.size());
    if (endString == " = 0,") {
      continue;
    }
    C8Log("%s", line.c_str());
  }
}

}  // namespace

void C8LogCapnpMessage(capnp::DynamicStruct::Reader value) {
  logAbbreviatedMessage(capnp::prettyPrint(value).flatten().cStr());
}

void C8LogCapnpMessage(capnp::DynamicList::Reader value) {
  logAbbreviatedMessage(capnp::prettyPrint(value).flatten().cStr());
}

}  // namespace c8
