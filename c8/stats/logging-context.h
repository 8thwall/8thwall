// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <capnp/message.h>

#include <chrono>
#include <memory>
#include <vector>

#include "c8/stats/api/detail.capnp.h"
#include "c8/string.h"

namespace c8 {

// A LoggingContext is a tree structure that loosely respresents a call stack. Nodes in the tree are
// added manually by callers, and can either be point events, or spans of execution.
//
// Typical usage might look like
//
// LoggingContext fooContext = loggingContext->createChild("compute-foo");
// amazingFooAlgorightm.doCoolFooComputation();
// fooContext.markCompletionTimepoint();
//
// Internally, this creates a new logging context which is a child of the current logging context,
// along with a tag and a start time. Calling markCompletionTimepoint() saves the current timestamp
// as an end point, which when paired with the start timestamp collected on construction can give a
// latency.
//
// Internal methods will typically want to take a LoggingContext as input so they can build up a
// call tree. This would typically look like:
//
// LoggingContext poseContext = loggingContext->createChild("slam-pose");
// slamPose.computePoseWithSlam([other arguments], &poseContext);
// poseContext.markCompletionTimepoint();
//
// This allows the slamPose object to add its own logging.
//
// Very rarely (e.g. in the top level execution code), you will want to create a new call tree using
// createRootLoggingTree. If you do this, and then don't actually do anything with the logging tree
// (like log it), then the logging operations are just a high memory no-op. So don't call
// createRootLoggingTree unless you know what you're doing with the logging tree after you construct
// it, and you're sure you shouldnt actually be logging to an existing tree.
class LoggingContext {
public:
  static constexpr const char SEPARATOR = '/';
  static constexpr const char *COUNTERS_PREFIX = "/counters/";
  /////////////////////////////// Factory Methods //////////////////////////////

  // Creates a new logging tree. This should only be called at the top level of
  // a call stack.
  static LoggingContext createRootLoggingTree(const String &tag);
  static std::unique_ptr<LoggingContext> createRootLoggingTreePtr(const String &tag);

  // Creates a new child node of the current LoggingContext.
  LoggingContext &createChild(const String &tag);

  ///////////////////////////// Annotating Context /////////////////////////////

  // Mark the current logging context as completed for timing purposes.
  void markCompletionTimepoint();

  // Add a named counter associated with the current event, along with an optional total possible
  // count to track percentages. E.g. if three features were matched out of fifty, count would be
  // three, and ofTotal would be fifty. In this case, ofTotal must be greater than or equal to
  // count. If you are just tracking a simple count, with no notion of percentage, just set ofTotal
  // to 0.
  void addCounter(const String &tag, uint64_t count, uint64_t ofTotal = 0);

  /////////////////////////////// Exporting Data ///////////////////////////////

  // Exports a detailed call tree as a flat list, with enough detail to
  // fully reconstruct the call tree.
  void exportDetail(LoggingDetail::Builder *detailBuilder) const;

  bool wasMarkedComplete() const;

  const String &tag() const { return fullTag_; }

  //////////////////////////////// Constructors ////////////////////////////////

  // Mark the current logging context as completed for timing purposes.

  // Default constructor is deleted. Use factory methods.
  LoggingContext() = delete;

  // Default move constructors.
  LoggingContext(LoggingContext &&) = default;
  LoggingContext &operator=(LoggingContext &&) = default;

  // Disallow copying.
  LoggingContext(const LoggingContext &) = delete;
  LoggingContext &operator=(const LoggingContext &) = delete;

private:
  String fullTag_;
  int64_t startTime_;
  std::vector<LoggingContext> children_;
  std::vector<LoggingContext> counters_;
  int64_t stopTime_;
  uint64_t count_ = 0;
  uint64_t ofTotal_ = 0;

  LoggingContext(const String &fullTag);

  struct EventDetailData {
    String eventName;
    String eventId;
    String parentId;
    int64_t startTimeMicros;
    bool hasEndTime = false;
    int64_t endTimeMicros;
    int64_t count;
    int64_t ofTotal;
  };

  void exportDetailInternal(const String &parentId, std::vector<EventDetailData> &detail) const;
};

}  // namespace c8
