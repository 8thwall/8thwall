// stackcollapse-logging-detail.cc
// Original Author: Pawel Czarnecki (pawel@8thwall.com)
// Copyright 8th Wall Inc. 2020

// This program ingests a packed LoggingDetail capnp binary file and emits a stackcollapse
// ready to be loaded by FlameGraph https://github.com/brendangregg/FlameGraph

// Sample usage:
//
// stackcollapse-logging-detail < packedFile.capnpbin > output.firegraph

#include "bzl/inliner/rules2.h"

cc_binary {
  visibility = {
    "//visibility:public",
  };
  deps = {
    "//c8/io:capnp-messages",
    "//c8:c8-log",
    "//c8:string",
    "//c8:vector",
    "//c8/stats/api:detail.capnp-cc",
  };
}
cc_end(0xdad525ae);

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include <unistd.h>
#include <fcntl.h>

#include "c8/string.h"
#include "c8/vector.h"
#include "c8/c8-log.h"
#include "c8/io/capnp-messages.h"
#include "c8/stats/api/detail.capnp.h"

using namespace c8;

int main(int argc, char *argv[]) {

  capnp::PackedFdMessageReader packedReader{STDIN_FILENO};
  MutableRootMessage<LoggingDetail> message;
  auto root = packedReader.getRoot<LoggingDetail>();
  message.setRoot(root);

  for (auto detail: message.reader().getEvents()) {
    auto length = detail.getEndTimeMicros() - detail.getStartTimeMicros();
    auto name = String(detail.getEventName().cStr());
    if (name.empty()) {
      name = "";
    }
    for (auto& c : name) {
      c = c == '/' ? ';' : c; // replace forward slash with semi
    }
    C8Log("%s %" PRId64, name.c_str() + 1, length); // skip first delimiter
  }

  return 0;
}
