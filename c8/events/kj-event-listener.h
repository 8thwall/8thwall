// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// EventListener implementation using the KJ (Capnproto) Async framework.

#pragma once

#include <map>

#include <kj/async-io.h>
#include <kj/async-unix.h>
#include <kj/async.h>
#include <kj/common.h>

#include "c8/events/event-listener.h"

namespace c8 {

class KjEventListener : public FdEventListener {
 public:
  KjEventListener();

  // Listen to a file descriptor and call callback whenever it becomes
  // available, subject to the provided EventFlags.
  void addFdEvent(
    int fd, EventFlags flags, std::function<void()> callback) override;

  // Remove a file descriptor from the list of events to observe. Silently
  // returns if fd is not in the list of observed events.
  void removeFdEvent(int fd) override;

  // Wait and continue to listen for events until stop() is called.
  void wait();

  // Stop waiting and pause event processing.
  void stop();

  // Mutable accessor for the kj::AsyncIoContext.
  kj::AsyncIoContext& mutableIoContext() { return ioContext; }

  // Disallow copy and assign.
  KjEventListener(const KjEventListener&) = delete;
  KjEventListener& operator=(const KjEventListener&) = delete;

 private:
  kj::AsyncIoContext ioContext;
  std::map<int, kj::Promise<void>> promises;
  kj::PromiseFulfillerPair<void> donePair;
};

}  // namespace c8
