// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)
//
// EventListener implementation using the C library libevent.

#pragma once

#include <event2/event.h>
#include <memory>
#include "c8/events/event-listener.h"
#include "c8/map.h"

namespace c8 {
struct EventCB {
  event* event;
  std::function<void()> callback;
};

/**
 * Since libevent uses evutil_socket_t as the ID, to conform with FdEventListener
 * interface, we will keep a map from socketId to evutil_socket_t.
 */
class LevEventListener : public FdEventListener {
 public:
  LevEventListener();

  // Listen to a socket ID and call callback whenever it becomes
  // available, subject to the provided EventFlags.
  void addFdEvent(
    FdInt fd, EventFlags flags, std::function<void()> callback) override;

  // Remove a socket ID from the list of events to observe. Silently
  // returns if fd is not in the list of observed events.
  void removeFdEvent(FdInt fd) override;

  // Wait and continue to listen for events until stop() is called.
  void wait();

  // Stop waiting and pause event processing.
  void stop();

  short convertToLevFlags(EventFlags flags);
  event_base* getEventBase() { return eventBase_; }

  // Create TCP socket using evutil_socketpair. AF_UNIX on UNIX, AF_INET on Windows.
  // Can be used with kjEventListener as well.
  static int createSocketPair(evutil_socket_t *sockets);

  // Disallow copy and assign.
  LevEventListener(const LevEventListener&) = delete;
  LevEventListener& operator=(const LevEventListener&) = delete;

  virtual ~LevEventListener();
 private:
  event_base *eventBase_;
  TreeMap<int, std::unique_ptr<EventCB>> fdIntToEvent_;
};

}  // namespace c8
