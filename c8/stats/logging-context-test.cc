// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":logging-context",
    "//c8/io:capnp-messages",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x32b826b9);

#include "c8/stats/logging-context.h"

#include <gtest/gtest.h>
#include "c8/io/capnp-messages.h"

using MutableLoggingDetail = c8::MutableRootMessage<c8::LoggingDetail>;

namespace c8 {

class LoggingContextTest : public ::testing::Test {};

TEST_F(LoggingContextTest, TestExportDetail) {
  LoggingContext root = LoggingContext::createRootLoggingTree("test");

  for (int i = 0; i < 3; ++i) {
    auto &fooChild = root.createChild("foo");
    fooChild.createChild("bar1").markCompletionTimepoint();
    fooChild.createChild("bar2").markCompletionTimepoint();
    fooChild.markCompletionTimepoint();
  }

  auto &bazChild = root.createChild("baz");
  bazChild.addCounter("counter1", 3, 2);
  bazChild.addCounter("counter2", 13, 15);
  bazChild.markCompletionTimepoint();

  root.markCompletionTimepoint();

  MutableLoggingDetail message;
  LoggingDetail::Builder detail = message.builder();

  root.exportDetail(&detail);

  EXPECT_EQ(13, detail.getEvents().size());
  EXPECT_STREQ("/test", detail.getEvents()[0].getEventName().cStr());
  EXPECT_STREQ("0", detail.getEvents()[0].getEventId().cStr());
  EXPECT_STREQ("", detail.getEvents()[0].getParentId().cStr());

  EXPECT_STREQ("/test/foo", detail.getEvents()[1].getEventName().cStr());
  EXPECT_STREQ("1", detail.getEvents()[1].getEventId().cStr());
  EXPECT_STREQ("0", detail.getEvents()[1].getParentId().cStr());

  EXPECT_STREQ("/test/foo/bar1", detail.getEvents()[2].getEventName().cStr());
  EXPECT_STREQ("2", detail.getEvents()[2].getEventId().cStr());
  EXPECT_STREQ("1", detail.getEvents()[2].getParentId().cStr());

  EXPECT_STREQ("/test/foo/bar2", detail.getEvents()[3].getEventName().cStr());
  EXPECT_STREQ("3", detail.getEvents()[3].getEventId().cStr());
  EXPECT_STREQ("1", detail.getEvents()[3].getParentId().cStr());

  EXPECT_STREQ("/test/foo", detail.getEvents()[4].getEventName().cStr());
  EXPECT_STREQ("4", detail.getEvents()[4].getEventId().cStr());
  EXPECT_STREQ("0", detail.getEvents()[4].getParentId().cStr());

  EXPECT_STREQ("/test/foo/bar1", detail.getEvents()[5].getEventName().cStr());
  EXPECT_STREQ("5", detail.getEvents()[5].getEventId().cStr());
  EXPECT_STREQ("4", detail.getEvents()[5].getParentId().cStr());

  EXPECT_STREQ("/test/foo/bar2", detail.getEvents()[6].getEventName().cStr());
  EXPECT_STREQ("6", detail.getEvents()[6].getEventId().cStr());
  EXPECT_STREQ("4", detail.getEvents()[6].getParentId().cStr());

  EXPECT_STREQ("/test/foo", detail.getEvents()[7].getEventName().cStr());
  EXPECT_STREQ("7", detail.getEvents()[7].getEventId().cStr());
  EXPECT_STREQ("0", detail.getEvents()[7].getParentId().cStr());

  EXPECT_STREQ("/test/foo/bar1", detail.getEvents()[8].getEventName().cStr());
  EXPECT_STREQ("8", detail.getEvents()[8].getEventId().cStr());
  EXPECT_STREQ("7", detail.getEvents()[8].getParentId().cStr());

  EXPECT_STREQ("/test/foo/bar2", detail.getEvents()[9].getEventName().cStr());
  EXPECT_STREQ("9", detail.getEvents()[9].getEventId().cStr());
  EXPECT_STREQ("7", detail.getEvents()[9].getParentId().cStr());

  EXPECT_STREQ("/test/baz", detail.getEvents()[10].getEventName().cStr());
  EXPECT_STREQ("10", detail.getEvents()[10].getEventId().cStr());
  EXPECT_STREQ("0", detail.getEvents()[10].getParentId().cStr());

  EXPECT_STREQ("/test/baz/counters/counter1", detail.getEvents()[11].getEventName().cStr());
  EXPECT_STREQ("11", detail.getEvents()[11].getEventId().cStr());
  EXPECT_STREQ("10", detail.getEvents()[11].getParentId().cStr());
  EXPECT_EQ(3, detail.getEvents()[11].getPositiveCount());
  EXPECT_EQ(0, detail.getEvents()[11].getOfTotalPositiveCount());

  EXPECT_STREQ("/test/baz/counters/counter2", detail.getEvents()[12].getEventName().cStr());
  EXPECT_STREQ("12", detail.getEvents()[12].getEventId().cStr());
  EXPECT_STREQ("10", detail.getEvents()[12].getParentId().cStr());
  EXPECT_EQ(13, detail.getEvents()[12].getPositiveCount());
  EXPECT_EQ(15, detail.getEvents()[12].getOfTotalPositiveCount());
  // TODO(nbutko): Add extra tests, like which events have end times, and the invariants around ids.
}

}  // namespace c8
