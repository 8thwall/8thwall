// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Event listener interfaces.

#pragma once

#include <functional>

namespace c8 {

#ifdef WIN32
  using FdInt = intptr_t;
#else
  using FdInt = int;
#endif

// Bitmask flags to pass to event listeners.
using EventFlags = unsigned;
struct EventFlag {
  // This flag indicates an event that becomes active when the provided file
  // descriptor is ready for reading.
  static constexpr EventFlags READ = 0x01;

  // This flag indicates an event that becomes active when the provided file
  // descriptor is ready for writing.
  static constexpr EventFlags WRITE = 0x02;

  // Indicates that the event is persistent.
  static constexpr EventFlags PERSIST = 0x4;

  // Indicates that the event should be edge-triggered, if the underlying
  // event_base backend supports edge-triggered events. This affects the
  // semantics of EV_READ and EV_WRITE.
  static constexpr EventFlags EDGE_TRIGGER = 0x8;
};

// Interface for observing a file descriptor.
class FdEventListener {
 public:

  // Add a fd to the list of events being observed.
  virtual void addFdEvent(
    FdInt fd, EventFlags flags, std::function<void()> callback) = 0;

  // Remove an fd from the list of events being observed.
  virtual void removeFdEvent(FdInt fd) = 0;
};

// Interface for listening on a signal.
class SignalEventListener {
 public:
  // Add a fd to the list of events being observed.
  virtual void addSignalEvent(
    FdInt fd, EventFlags flags, std::function<void()> callback) = 0;

  // Remove an fd from the list of events being observed.
  virtual void removeSignalEvent(FdInt fd) = 0;
};

// Interface for listening on a timer.
class TimerEventListener {
 public:
  // Add a timer to the list of events being observed.
  virtual void addTimerEvent(
    FdInt fd, EventFlags flags, std::function<void()> callback) = 0;

  // Remove a time from the list of events being observed.
  virtual void removeTimerEvent(FdInt fd) = 0;
};

}  // namespace c8
