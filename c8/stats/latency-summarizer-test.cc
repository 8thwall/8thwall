// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":latency-summarizer",
    ":logging-context",
    "//c8/io:capnp-messages",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xfd8d4b07);

#include "c8/stats/latency-summarizer.h"

#include <gtest/gtest.h>
#include "c8/io/capnp-messages.h"
#include "c8/stats/logging-context.h"

using MutableLoggingDetail = c8::MutableRootMessage<c8::LoggingDetail>;
using MutableLoggingSummary = c8::MutableRootMessage<c8::LoggingSummary>;

namespace c8 {

class LatencySummarizerTest : public ::testing::Test {};

TEST_F(LatencySummarizerTest, TestExportSummary) {
  LoggingContext root = LoggingContext::createRootLoggingTree("test");

  for (int i = 0; i < 12; ++i) {
    LoggingContext &fooChild = root.createChild("foo");
    auto &c1 = fooChild.createChild("bar1");
    c1.addCounter("countA", 3, 0);
    c1.markCompletionTimepoint();
    auto &c2 = fooChild.createChild("bar2");
    c2.addCounter("countB", 5, 10);
    c2.markCompletionTimepoint();
    fooChild.markCompletionTimepoint();
  }

  root.createChild("baz").markCompletionTimepoint();
  root.markCompletionTimepoint();

  MutableLoggingDetail detailMessage;
  LoggingDetail::Builder detail = detailMessage.builder();
  root.exportDetail(&detail);

  LatencySummarizer summarizer;
  for (int i = 0; i < 5; ++i) {
    summarizer.summarize(detail);
  }

  MutableLoggingSummary summaryMessage;
  LoggingSummary::Builder summary = summaryMessage.builder();
  summarizer.exportSummary(&summary);

  auto events = summary.getEvents();

  // There are five events, ordered lexicographically.
  EXPECT_EQ(7, events.size());
  EXPECT_STREQ("/test", events[0].getEventName().cStr());
  EXPECT_STREQ("/test/baz", events[1].getEventName().cStr());
  EXPECT_STREQ("/test/foo", events[2].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar1", events[3].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar1/counters/countA", events[4].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar2", events[5].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar2/counters/countB", events[6].getEventName().cStr());

  // There are 5 events for the first two stats, and 60 for the next three.
  EXPECT_EQ(5, events[0].getLatencyMicros().getMeanStats().getNumDataPoints());

  EXPECT_EQ(5, events[1].getEventCount());
  EXPECT_EQ(5, events[1].getLatencyMicros().getMeanStats().getNumDataPoints());

  EXPECT_EQ(60, events[2].getLatencyMicros().getMeanStats().getNumDataPoints());

  EXPECT_EQ(60, events[3].getEventCount());
  EXPECT_EQ(60, events[3].getLatencyMicros().getMeanStats().getNumDataPoints());

  EXPECT_EQ(60, events[4].getEventCount());
  EXPECT_EQ(0, events[4].getLatencyMicros().getMeanStats().getNumDataPoints());
  EXPECT_EQ(60, events[4].getPositiveCount().getMeanStats().getNumDataPoints());
  EXPECT_EQ(0, events[4].getOfTotalPositiveCount().getMeanStats().getNumDataPoints());

  EXPECT_EQ(60, events[5].getEventCount());
  EXPECT_EQ(60, events[5].getLatencyMicros().getMeanStats().getNumDataPoints());

  EXPECT_EQ(60, events[6].getEventCount());
  EXPECT_EQ(0, events[6].getLatencyMicros().getMeanStats().getNumDataPoints());
  EXPECT_EQ(60, events[6].getPositiveCount().getMeanStats().getNumDataPoints());
  EXPECT_EQ(60, events[6].getOfTotalPositiveCount().getMeanStats().getNumDataPoints());

  // Events 0, 1, 2, 3, and 5 have logged time sums, the rest don't.
  EXPECT_LE(0, events[0].getLatencyMicros().getMeanStats().getSum());
  EXPECT_LE(0, events[1].getLatencyMicros().getMeanStats().getSum());
  EXPECT_LE(0, events[2].getLatencyMicros().getMeanStats().getSum());
  EXPECT_LE(0, events[3].getLatencyMicros().getMeanStats().getSum());
  EXPECT_EQ(0, events[4].getLatencyMicros().getMeanStats().getSum());
  EXPECT_LE(0, events[5].getLatencyMicros().getMeanStats().getSum());
  EXPECT_EQ(0, events[6].getLatencyMicros().getMeanStats().getSum());
}

}  // namespace c8
