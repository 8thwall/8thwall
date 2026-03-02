// Copyright (c) 2017 8th Wall, LLC.
// Original Author: Pawel Czarnecki (pawel@niantic.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":export-detail",
    "//c8/io:capnp-messages",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x81d01a25);

#include <gtest/gtest.h>

#include "c8/io/capnp-messages.h"
#include "c8/stats/export-detail.h"

using MutableLoggingDetail = c8::MutableRootMessage<c8::LoggingDetail>;

namespace c8 {

class ExportDetailTest : public ::testing::Test {};

//                                               61
//                                               │
//                                    46  47     ▼   69 83     94
//                                     │  │      ┌────┐
//       The numbers in the paren is   │  ▼     R│8(8)│
// what  flamegraph expects            │  ┌──┐   ├────┤ │      │
//            9         25             ▼  │5P│  Q│8(0)│ ▼      ▼    102
//            │         │            45┌──┴──┤   ├────┤ ┌──────┬─────┐
//            ▼         ▼             ▼│6(1)L│  M│8(0)│ │N 11  │O 8  │
//       3    ┌───────┐ ┌───────────┐ ┌┴─────┴┐ ┌┴────┤ ├──────┴─────┤
//       ▲    │  14 G │ │    18  H  │ │I 10(4)│ │9J(1)│ │ K   19 (0) │
//       ┌────┴───────┼─┴───────────┼─┴───────┴─┴─────┴─┼────────────┴┐
//       │ 20 (6)   C │    20 (2) D │       40    E     │    20 (1) F │
//       ├────────────┴─────────────┴───────────────────┴─────────────┤
//       │3--                     100(0)            B             -103│
//     ┌─┴────────────────────────────────────────────────────────────┴┐
//     │0--                    105 (5)              A             --100│
//     └───────────────────────────────────────────────────────────────┘
//     ▲              ▲             ▲        ▲ ▲ ▲
//     │              │             │        │ │ │
//     0              23            43      52 55 60
//
//           18 details

struct Detail {
  String eventId;
  String parentId;
  uint64_t start;
  uint64_t end;
};

void setCoreDetail(auto builder, auto &detail) {
  builder.setEventName(detail.eventId);
  builder.setEventId(detail.eventId);
  builder.setParentId(detail.parentId);
  builder.setStartTimeMicros(detail.start);
  builder.setEndTimeMicros(detail.end);
}

TEST_F(ExportDetailTest, FlameGraph1) {
  MutableLoggingDetail details;
  auto builder = details.builder();

  Vector<Detail> coreDetails = {
    {"a", "", 0, 105},
    {"b", "a", 3, 103},
    {"c", "b", 3, 23},
    {"g", "c", 9, 23},
    {"d", "b", 23, 43},
    {"h", "d", 25, 43},
    {"e", "b", 43, 83},
    {"i", "e", 45, 55},
    {"l", "i", 46, 52},
    {"p", "l", 47, 52},
    {"j", "e", 60, 69},
    {"m", "j", 61, 69},
    {"q", "m", 61, 69},
    {"r", "q", 61, 69},
    {"f", "b", 83, 103},
    {"k", "f", 83, 102},
    {"n", "k", 83, 94},
    {"o", "k", 94, 102},
  };

  HashMap<String, uint64_t> expectedLengths = {
    {"a", 5},
    {"b", 0},
    {"c", 6},
    {"d", 2},
    {"e", 21},
    {"f", 1},
    {"g", 14},
    {"h", 18},
    {"i", 4},
    {"j", 1},
    {"k", 0},
    {"l", 1},
    {"m", 0},
    {"n", 11},
    {"o", 8},
    {"p", 5},
    {"q", 0},
    {"r", 8},
  };

  builder.initEvents(coreDetails.size());
  for (int i = 0; i < coreDetails.size(); i++) {
    setCoreDetail(builder.getEvents()[i], coreDetails[i]);
  }

  auto durations = computeFlamegraphValues(details.reader());
  for (auto detail : details.reader().getEvents()) {
    String eventId = detail.getEventId();
    EXPECT_EQ(durations[eventId], expectedLengths[eventId]);
  }
}
}  // namespace c8
