// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)
//
// Utility class to prepare logs to be uploaded to the server.
// This may involve things such as: serialization, conversion to JSON, compression, etc.

#pragma once

#include <capnp/compat/json.h>
#include <capnp/serialize.h>

#include "c8/map.h"
#include "c8/protolog/api/log-request.capnp.h"
#include "c8/stats/latency-summarizer.h"
#include "c8/stats/logging-context.h"
#include "c8/string.h"
#include "reality/engine/api/device/info.capnp.h"

namespace c8 {

const Vector<String> &metrics();

class Timer {
public:
  void on();
  // The elapsed time since the most recent on() or reset().
  int64_t current() const;
  void off();

  // Pause is used to pause accruing time while leaving the timer in either an off or on state.
  // - If you pause() and then unpause() something that's off, it will end up off.
  // - If you pause() and then unpause() something that's on, it will end up on.
  // - If you are on while paused, it won't accrue anything until you unpause(), then it will accrue
  // - If you are off while paused, when you unpause(), it won't acrue anything until you on().
  void pause();
  void unpause();

  bool isPaused() const { return pauseTime_ != 0; };
  bool isOff() const { return onTime_ == 0; };
  bool isOn() const { return !isOff(); };

  // Set our current value to zero so it can start accruing again.
  void reset();

  // Public virtual so that a unit test mock timer can override.
  virtual int64_t getNowMicros() const;

private:
  // If not equal to 0 then the on timer is running, and onTime_ is when it started.
  int64_t onTime_ = 0;
  // How long we've been on for, excluding the on timer that may be currently running.
  int64_t total_ = 0;

  // If not equal to 0 then the pause timer is running, and pauseTime_ is when it started.
  int64_t pauseTime_ = 0;
  // How long we've been paused for, excluding the pause timer that may be currently running.
  int64_t currentPauseTotal_ = 0;
};

class XRSessionStats {
public:
  void initializeTimers();
  void reset();
  void exportRecord(LogRecord::Builder logRecord);

  void incrementFacesFound();
  void incrementHandsFound();
  void incrementImageTargetsFound();
  void incrementImageTargetsFoundUnique();
  void incrementWayspotAnchorsFound();
  void incrementWayspotAnchorsFoundUnique();
  void incrementWayspotAnchorsUpdated();
  void incrementNodesFound();
  void incrementNodesFoundUnique();
  void incrementNodesUpdated();
  void pauseAll();
  void unpauseAll();
  void stopAll();

  XRSessionStats() { initializeTimers(); };
  Timer &timer(const String &t) { return timing_[t]; }

private:
  TreeMap<String, Timer> timing_;

  int64_t facesFoundTotal_ = 0;
  int64_t handsFoundTotal_ = 0;
  int64_t imageTargetsFoundTotal_ = 0;
  int64_t imageTargetsFoundUnique_ = 0;
  int64_t wayspotAnchorsFoundTotal_ = 0;
  int64_t wayspotAnchorsFoundUnique_ = 0;
  int64_t wayspotAnchorsUpdated_ = 0;
  int64_t nodesFoundTotal_ = 0;
  int64_t nodesFoundUnique_ = 0;
  int64_t nodesUpdated_ = 0;
};

class XRLogPreparer {
public:
  // Default constructor.
  XRLogPreparer() = default;
  ~XRLogPreparer() = default;

  // Default move constructors.
  XRLogPreparer(XRLogPreparer &&) = default;
  XRLogPreparer &operator=(XRLogPreparer &&) = default;

  // Disallow copying.
  XRLogPreparer(const XRLogPreparer &) = delete;
  XRLogPreparer &operator=(const XRLogPreparer &) = delete;

  // Prepares a log record to be uploaded to the server. This will combine the provided data to
  // construct a LogServiceRequest capnp struct. This struct is then serialized in a packed format,
  // then compressed. A vector to the resulting bytes is then returned.
  std::unique_ptr<Vector<uint8_t>> prepareLogForUpload(
    const LogRecordHeader::Reader &logRecordHeader, LatencySummarizer *summarizer);

  void startNewLoggingSession();

  void endLoggingSession(LatencySummarizer *summarizer);

  XRSessionStats &stats() { return stats_; };

private:
  std::unique_ptr<LoggingContext> sessionLoggingContext_;
  XRSessionStats stats_;
};

}  // namespace c8
