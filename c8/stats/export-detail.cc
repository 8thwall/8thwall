// Copyright (c) 2017 8th Wall, LLC.
// Original Author: Pawel Czarnecki (pawel@niantic.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "export-detail.h",
  };
  deps = {
    "//c8:vector",
    "//c8:map",
    "//c8:set",
    "//c8:string",
    "//c8/io:capnp-messages",
    "//c8/stats:logging-context",
    "//c8/stats:scope-timer",
    "//c8/stats/api:detail.capnp-cc",
  };
}
cc_end(0xbf189526);

#include <sstream>
#include "c8/io/capnp-messages.h"
#include "c8/stats/export-detail.h"
#include "c8/stats/scope-timer.h"

using namespace c8;

namespace c8 {

namespace {
inline void replaceChar(String &haystack, char target, char replacement) {
  for (char &c : haystack) {
    if (c == target) {
      c = replacement;
    }
  }
}
}

HashMap<String, uint64_t> computeFlamegraphValues(const LoggingDetail::Reader &reader) {
  HashMap<String, uint64_t> durations;
  for (auto detail : reader.getEvents()) {
    uint64_t duration = detail.getEndTimeMicros() - detail.getStartTimeMicros();
    durations[detail.getEventId().cStr()] += duration;
    durations[detail.getParentId().cStr()] -= duration;
  }
  durations.erase("");
  return durations;
}

// Get text that can be interpreted by the flamegraph.pl utility (brew install flamegraph) and used
// for generating an svg.
String flamegraphText(LoggingContext *lc) {
  if (lc == nullptr) {
    lc = ScopeTimer::lastCompleted();
  }
  if (lc == nullptr) {
    return "";
  }
  std::stringstream ss;

  MutableRootMessage<LoggingDetail> detailRoot;
  auto detailBuilder = detailRoot.builder();
  lc->exportDetail(&detailBuilder);

  auto adjustedDurations = computeFlamegraphValues(detailRoot.reader());

  for (auto detail : detailRoot.reader().getEvents()) {
    String line(&detail.getEventName().cStr()[1]);  // ub if event name is empty.
    replaceChar(line, '/', ';');
    line += " ";
    line += std::to_string(adjustedDurations[detail.getEventId().cStr()]);
    ss << line.c_str() << "\n";
  }

  return ss.str();
}

}  // namespace c8
