// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":scope-timer",
    "//c8/io:capnp-messages",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xc714bb27);

#include <gtest/gtest.h>

#include "c8/io/capnp-messages.h"
#include "c8/stats/scope-timer.h"

using MutableLoggingDetail = c8::MutableRootMessage<c8::LoggingDetail>;

namespace c8 {

class ScopeTimerTest : public ::testing::Test {};

TEST_F(ScopeTimerTest, TestScopeTimer) {
  {
    ScopeTimer t("test");

    for (int i = 0; i < 3; ++i) {
      ScopeTimer t2("foo");
      { ScopeTimer t3("bar1"); }
      { ScopeTimer t3("bar2"); }
    }
  }

  MutableLoggingDetail message;
  auto detail = message.builder();

  ScopeTimer::lastCompleted()->exportDetail(&detail);

  EXPECT_EQ(10, detail.getEvents().size());
  EXPECT_STREQ("/test", detail.getEvents()[0].getEventName().cStr());
  EXPECT_STREQ("/test/foo", detail.getEvents()[1].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar1", detail.getEvents()[2].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar2", detail.getEvents()[3].getEventName().cStr());
  EXPECT_STREQ("/test/foo", detail.getEvents()[4].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar1", detail.getEvents()[5].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar2", detail.getEvents()[6].getEventName().cStr());
  EXPECT_STREQ("/test/foo", detail.getEvents()[7].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar1", detail.getEvents()[8].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar2", detail.getEvents()[9].getEventName().cStr());

  // All stacks have been popped.
  EXPECT_FALSE(ScopeTimer::hasCurrent());
}

TEST_F(ScopeTimerTest, TestScopeTimerResetRoot) {
  {
    ScopeTimer t("test");

    for (int i = 0; i < 3; ++i) {
      ScopeTimer t2("foo");
      { ScopeTimer t3("bar1", true); }
      { ScopeTimer t3("bar2"); }
    }
  }

  MutableLoggingDetail message;
  auto detail = message.builder();

  ScopeTimer::lastCompleted()->exportDetail(&detail);

  EXPECT_EQ(7, detail.getEvents().size());
  EXPECT_STREQ("/test", detail.getEvents()[0].getEventName().cStr());
  EXPECT_STREQ("/test/foo", detail.getEvents()[1].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar2", detail.getEvents()[2].getEventName().cStr());
  EXPECT_STREQ("/test/foo", detail.getEvents()[3].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar2", detail.getEvents()[4].getEventName().cStr());
  EXPECT_STREQ("/test/foo", detail.getEvents()[5].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar2", detail.getEvents()[6].getEventName().cStr());

  // All stacks have been popped.
  EXPECT_FALSE(ScopeTimer::hasCurrent());
}

TEST_F(ScopeTimerTest, TestScopeTimerSummarizer) {
  ScopeTimer::reset();
  for (int i = 0; i < 5; ++i) {
    ScopeTimer t("test");

    for (int j = 0; j < 12; ++j) {
      ScopeTimer t2("foo");
      {
        ScopeTimer t3("bar1");
        t3.addCounter("countA", 3);
      }
      {
        ScopeTimer t3("bar2");
        t3.addCounter("countB", 5, 10);
      }
    }

    { ScopeTimer t2("baz"); }
  }

  MutableRootMessage<LoggingSummary> summaryMessage;
  LoggingSummary::Builder summary = summaryMessage.builder();
  ScopeTimer::exportSummary(&summary);

  auto events = summary.getEvents();

  // There are five events, ordered lexicographically.
  EXPECT_EQ(10, events.size());
  EXPECT_STREQ("/summarizer", events[0].getEventName().cStr());
  EXPECT_STREQ("/summarizer/test", events[1].getEventName().cStr());
  EXPECT_STREQ("/summarizer/test/counters/num-counters", events[2].getEventName().cStr());
  EXPECT_STREQ("/test", events[3].getEventName().cStr());
  EXPECT_STREQ("/test/baz", events[4].getEventName().cStr());
  EXPECT_STREQ("/test/foo", events[5].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar1", events[6].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar1/counters/countA", events[7].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar2", events[8].getEventName().cStr());
  EXPECT_STREQ("/test/foo/bar2/counters/countB", events[9].getEventName().cStr());

  // There are 5 events for the first two stats, and 60 for the next three.
  EXPECT_EQ(5, events[3].getLatencyMicros().getMeanStats().getNumDataPoints());

  EXPECT_EQ(5, events[4].getEventCount());
  EXPECT_EQ(5, events[4].getLatencyMicros().getMeanStats().getNumDataPoints());

  EXPECT_EQ(60, events[5].getLatencyMicros().getMeanStats().getNumDataPoints());

  EXPECT_EQ(60, events[6].getEventCount());
  EXPECT_EQ(60, events[6].getLatencyMicros().getMeanStats().getNumDataPoints());

  EXPECT_EQ(60, events[7].getEventCount());
  EXPECT_EQ(0, events[7].getLatencyMicros().getMeanStats().getNumDataPoints());
  EXPECT_EQ(60, events[7].getPositiveCount().getMeanStats().getNumDataPoints());
  EXPECT_EQ(0, events[7].getOfTotalPositiveCount().getMeanStats().getNumDataPoints());

  EXPECT_EQ(60, events[8].getEventCount());
  EXPECT_EQ(60, events[8].getLatencyMicros().getMeanStats().getNumDataPoints());

  EXPECT_EQ(60, events[9].getEventCount());
  EXPECT_EQ(0, events[9].getLatencyMicros().getMeanStats().getNumDataPoints());
  EXPECT_EQ(60, events[9].getPositiveCount().getMeanStats().getNumDataPoints());
  EXPECT_EQ(60, events[9].getOfTotalPositiveCount().getMeanStats().getNumDataPoints());

  // Counters don't have latency.
  EXPECT_LE(0, events[2].getLatencyMicros().getMeanStats().getSum());
  EXPECT_LE(0, events[7].getLatencyMicros().getMeanStats().getSum());
  EXPECT_LE(0, events[9].getLatencyMicros().getMeanStats().getSum());

  // All stacks have been popped.
  EXPECT_FALSE(ScopeTimer::hasCurrent());
}

TEST_F(ScopeTimerTest, TestScopeTimerSummarizerResetRoot) {
  ScopeTimer::reset();
  for (int i = 0; i < 5; ++i) {
    ScopeTimer t("test", true);  // 'true' here is unneeded but shouldn't hurt anything.

    for (int j = 0; j < 12; ++j) {
      ScopeTimer t2("foo", true);
      {
        ScopeTimer t3("bar1");
        t3.addCounter("countA", 3);
      }
      {
        ScopeTimer t3("bar2");
        t3.addCounter("countB", 5, 10);
      }
    }

    { ScopeTimer t2("baz"); }
  }

  MutableRootMessage<LoggingSummary> summaryMessage;
  LoggingSummary::Builder summary = summaryMessage.builder();
  ScopeTimer::exportSummary(&summary);

  auto events = summary.getEvents();

  // There are five events, ordered lexicographically.
  EXPECT_EQ(12, events.size());
  EXPECT_STREQ("/foo", events[0].getEventName().cStr());
  EXPECT_STREQ("/foo/bar1", events[1].getEventName().cStr());
  EXPECT_STREQ("/foo/bar1/counters/countA", events[2].getEventName().cStr());
  EXPECT_STREQ("/foo/bar2", events[3].getEventName().cStr());
  EXPECT_STREQ("/foo/bar2/counters/countB", events[4].getEventName().cStr());
  EXPECT_STREQ("/summarizer", events[5].getEventName().cStr());
  EXPECT_STREQ("/summarizer/foo", events[6].getEventName().cStr());
  EXPECT_STREQ("/summarizer/foo/counters/num-counters", events[7].getEventName().cStr());
  EXPECT_STREQ("/summarizer/test", events[8].getEventName().cStr());
  EXPECT_STREQ("/summarizer/test/counters/num-counters", events[9].getEventName().cStr());
  EXPECT_STREQ("/test", events[10].getEventName().cStr());
  EXPECT_STREQ("/test/baz", events[11].getEventName().cStr());

  // There are 5 events for the 'test' and 'baz' stats, and 60 for the 'foo' tree.
  EXPECT_EQ(60, events[0].getLatencyMicros().getMeanStats().getNumDataPoints());

  EXPECT_EQ(60, events[1].getEventCount());
  EXPECT_EQ(60, events[1].getLatencyMicros().getMeanStats().getNumDataPoints());

  EXPECT_EQ(60, events[2].getEventCount());
  EXPECT_EQ(0, events[2].getLatencyMicros().getMeanStats().getNumDataPoints());
  EXPECT_EQ(60, events[2].getPositiveCount().getMeanStats().getNumDataPoints());
  EXPECT_EQ(0, events[2].getOfTotalPositiveCount().getMeanStats().getNumDataPoints());

  EXPECT_EQ(60, events[3].getEventCount());
  EXPECT_EQ(60, events[3].getLatencyMicros().getMeanStats().getNumDataPoints());

  EXPECT_EQ(60, events[4].getEventCount());
  EXPECT_EQ(0, events[4].getLatencyMicros().getMeanStats().getNumDataPoints());
  EXPECT_EQ(60, events[4].getPositiveCount().getMeanStats().getNumDataPoints());
  EXPECT_EQ(60, events[4].getOfTotalPositiveCount().getMeanStats().getNumDataPoints());

  EXPECT_EQ(5, events[10].getLatencyMicros().getMeanStats().getNumDataPoints());

  EXPECT_EQ(5, events[11].getEventCount());
  EXPECT_EQ(5, events[11].getLatencyMicros().getMeanStats().getNumDataPoints());

  // Counters don't have latency.
  EXPECT_LE(0, events[2].getLatencyMicros().getMeanStats().getSum());
  EXPECT_LE(0, events[4].getLatencyMicros().getMeanStats().getSum());
  EXPECT_LE(0, events[7].getLatencyMicros().getMeanStats().getSum());
  EXPECT_LE(0, events[9].getLatencyMicros().getMeanStats().getSum());

  // All stacks have been popped.
  EXPECT_FALSE(ScopeTimer::hasCurrent());
}

}  // namespace c8
