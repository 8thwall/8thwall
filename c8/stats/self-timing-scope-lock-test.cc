// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":self-timing-scope-lock",
    ":scope-timer",
    "//c8/io:capnp-messages",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x706fbad7);

#include <gtest/gtest.h>

#include "c8/io/capnp-messages.h"
#include "c8/stats/self-timing-scope-lock.h"

using MutableLoggingDetail = c8::MutableRootMessage<c8::LoggingDetail>;

namespace c8 {

class SelfTimingScopeLockTest : public ::testing::Test {};

TEST_F(SelfTimingScopeLockTest, TestScopeTimmer) {
  {
    ScopeTimer rt("test");

    std::mutex m;
    for (int i = 0; i < 3; ++i) {
      ScopeTimingScopeLock t("lock-foo", m);
      t.loggingContext()->createChild("bar1").markCompletionTimepoint();
      t.loggingContext()->createChild("bar2").markCompletionTimepoint();
    }
  }

  MutableLoggingDetail message;
  LoggingDetail::Builder detail = message.builder();

  ScopeTimer::lastCompleted()->exportDetail(&detail);

  EXPECT_EQ(16, detail.getEvents().size());
  EXPECT_STREQ("/test", detail.getEvents()[0].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo", detail.getEvents()[1].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo/acquire-lock", detail.getEvents()[2].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo/hold-lock", detail.getEvents()[3].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo/hold-lock/bar1", detail.getEvents()[4].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo/hold-lock/bar2", detail.getEvents()[5].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo", detail.getEvents()[6].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo/acquire-lock", detail.getEvents()[7].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo/hold-lock", detail.getEvents()[8].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo/hold-lock/bar1", detail.getEvents()[9].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo/hold-lock/bar2", detail.getEvents()[10].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo", detail.getEvents()[11].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo/acquire-lock", detail.getEvents()[12].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo/hold-lock", detail.getEvents()[13].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo/hold-lock/bar1", detail.getEvents()[14].getEventName().cStr());
  EXPECT_STREQ("/test/lock-foo/hold-lock/bar2", detail.getEvents()[15].getEventName().cStr());
}

}  // namespace c8
