// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"
#include <cmath>

cc_test {
  size = "small";
  deps = {
    ":kj-event-listener", "//bzl/inliner:rules", "@com_google_googletest//:gtest_main"
  };
}
cc_end(0xc1d764fb);

#include <kj/debug.h>
#include "c8/events/kj-event-listener.h"
#include "c8/events/event-listener.h"
#include "gtest/gtest.h"

namespace c8 {

class KjEventListenerTest : public ::testing::Test {};

TEST_F(KjEventListenerTest, TestSimpleHelloWorldEvent) {
  // We set up a pipe and write data to it
  // KjBackedListener should be able to read out the exact content.
  KjEventListener eventListener;
  int pipefds[2];
  KJ_SYSCALL(pipe(pipefds));
  int readFd = pipefds[0];
  int writeFd = pipefds[1];

  // write data to the pipe so the event loop can pick it up
  std::string str_to_write = "Hello, world! Hola, mundo!";

  int invokeCount = 0;
  eventListener.addFdEvent(readFd, EventFlag::READ | EventFlag::EDGE_TRIGGER | EventFlag::PERSIST,
    [&invokeCount, &eventListener, readFd, &str_to_write] () mutable {
      std::unique_ptr<char[]> readBuffer(new char[str_to_write.size() + 1]);
      read(readFd, readBuffer.get(), str_to_write.size() + 1);
      EXPECT_STREQ(str_to_write.c_str(), readBuffer.get());

      invokeCount++;

      eventListener.stop();
    }
  );

  write(writeFd, str_to_write.c_str(), str_to_write.size() + 1);

  // Let the event loop run
  eventListener.wait();

  EXPECT_EQ(1, invokeCount);

  eventListener.removeFdEvent(readFd);
  close(readFd);
  close(writeFd);
}

}  // namespace c8
