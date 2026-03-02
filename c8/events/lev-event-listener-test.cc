// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"
#include <cmath>

cc_test {
  size = "small";
  deps = {
    ":lev-event-listener", "//bzl/inliner:rules", "@com_google_googletest//:gtest_main"
  };
}
cc_end(0x72483bde);

#include "c8/events/lev-event-listener.h"
#include "c8/events/event-listener.h"
#include "gtest/gtest.h"

namespace c8 {

class LevEventListenerTest : public ::testing::Test {};

TEST_F(LevEventListenerTest, TestConvertFlags) {
  LevEventListener listener;
  EXPECT_EQ(EV_READ, listener.convertToLevFlags(EventFlag::READ));

  short levFlags = listener.convertToLevFlags(EventFlag::READ | EventFlag::PERSIST);
  EXPECT_TRUE(levFlags & EV_READ);
  EXPECT_TRUE(levFlags & EV_PERSIST);
  EXPECT_FALSE(levFlags & EV_WRITE);
  EXPECT_FALSE(levFlags & EV_ET);
  EXPECT_FALSE(levFlags & EV_SIGNAL);
}

TEST_F(LevEventListenerTest, TestSimpleHelloWorldEvent) {
  char dataString[] = "Some interesting data";
  LevEventListener listener;

  // Socketpair allows inter-thread communications. Make sure to use a real socket if you want INET
  // communication.
  evutil_socket_t sockets[2];
  if (LevEventListener::createSocketPair(sockets) < 0) {
    FAIL() << "Unable to open socket pair. " << evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR());
  }

  // Set up reading from one socket
  int listenSocketId = sockets[0];
  int writeSocketId = sockets[1];
  evutil_make_socket_nonblocking(listenSocketId);

  // Listen for reading events
  int invokeCount = 0;
  listener.addFdEvent(listenSocketId, EventFlag::READ | EventFlag::PERSIST,
    [dataString, listenSocketId, &invokeCount, &listener] () {
      std::unique_ptr<char[]> readBuffer(new char[sizeof(dataString)]);
      read(listenSocketId, readBuffer.get(), sizeof(dataString));
      EXPECT_STREQ(dataString, readBuffer.get());

      ++invokeCount;
      listener.stop();
    });

  // Write data to the other socket
  write(writeSocketId, dataString, sizeof(dataString));

  listener.wait();

  EXPECT_EQ(1, invokeCount);
  listener.removeFdEvent(listenSocketId);
  close(listenSocketId);
  close(writeSocketId);
}

}  // namespace c8
