// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "logging-context.h",
  };
  deps = {
    "//c8:exceptions",
    "//c8:string",
    "//c8/time:now",
    "//c8/stats/api:detail.capnp-cc",
  };
}
cc_end(0x7d43a744);

#include "c8/exceptions.h"
#include "c8/stats/logging-context.h"
#include "c8/time/now.h"

namespace {

// Replace '/' with '-' in the user entered tags for LoggingContext.
void escapeSlash(c8::String &str) {
  for (int i = 0; i < str.size(); ++i) {
    if (str[i] == c8::LoggingContext::SEPARATOR) {
      str[i] = '-';
    }
  }
}

}  // namespace

namespace c8 {

LoggingContext LoggingContext::createRootLoggingTree(const String &tag) {
  String escapedTag = tag;
  escapeSlash(escapedTag);
  LoggingContext newContext(SEPARATOR + escapedTag);
  return newContext;
}

std::unique_ptr<LoggingContext> LoggingContext::createRootLoggingTreePtr(const String &tag) {
  String escapedTag = tag;
  escapeSlash(escapedTag);
  return std::unique_ptr<LoggingContext>(new LoggingContext(SEPARATOR + escapedTag));
}

LoggingContext &LoggingContext::createChild(const String &tag) {
  if (!children_.empty() && !children_.back().wasMarkedComplete()) {
    C8_THROW(
      String("Calling create child with new tag ") + tag + " from logging context " + fullTag_
      + " before previous child context " + children_.back().fullTag_ + " was marked complete.");
  }
  if (wasMarkedComplete()) {
    C8_THROW(
      String("Calling create child with new tag ") + tag + " from logging context " + fullTag_
      + " after it was already was marked complete.");
  }
  String escapedTag = tag;
  escapeSlash(escapedTag);
  LoggingContext newChild(fullTag_ + SEPARATOR + escapedTag);
  children_.push_back(std::move(newChild));
  return children_.back();
}

void LoggingContext::markCompletionTimepoint() {
  if (wasMarkedComplete()) {
    C8_THROW(
      String("Duplicate call to markCompletionTimepoint from logging context ") + fullTag_
      + " after it was already was marked complete.");
  }
  if (!children_.empty() && !children_.back().wasMarkedComplete()) {
    C8_THROW(
      String("Calling markCompletionTimepoint from logging context ") + fullTag_
      + " before previous child context " + children_.back().fullTag_ + " was marked complete.");
  }
  stopTime_ = nowMicros();
}

void LoggingContext::addCounter(const String &tag, uint64_t count, uint64_t ofTotal) {
  if (wasMarkedComplete()) {
    C8_THROW(
      String("Calling add counter with new tag ") + tag + " from logging context " + fullTag_
      + " after it was already was marked complete.");
  }
  String escapedTag = tag;
  escapeSlash(escapedTag);
  LoggingContext counter(fullTag_ + COUNTERS_PREFIX + escapedTag);
  counter.count_ = count;
  counter.ofTotal_ = ofTotal < count ? 0 : ofTotal;
  counters_.push_back(std::move(counter));
}

void LoggingContext::exportDetail(LoggingDetail::Builder *detailBuilder) const {
  std::vector<EventDetailData> detail;
  exportDetailInternal("", detail);

  auto events = detailBuilder->initEvents(detail.size());
  for (int i = 0; i < detail.size(); ++i) {
    auto event = events[i];
    const auto &data = detail[i];
    event.setEventName(data.eventName);
    event.setEventId(data.eventId);
    event.setParentId(data.parentId);

    event.setStartTimeMicros(data.startTimeMicros);
    if (data.hasEndTime) {
      event.setEndTimeMicros(data.endTimeMicros);
    }
    event.setPositiveCount(data.count);
    event.setOfTotalPositiveCount(data.ofTotal);
  }
}

LoggingContext::LoggingContext(const String &fullTag)
    : fullTag_(fullTag),
      startTime_(nowMicros()),
      children_(),
      stopTime_(0),
      count_(0),
      ofTotal_(0) {}

void LoggingContext::exportDetailInternal(
  const String &parentId, std::vector<EventDetailData> &detail) const {
  if (!children_.empty()) {
    if (!children_.back().wasMarkedComplete()) {
      C8_THROW(
        String("Calling exportDetail from logging context ") + fullTag_
        + " before previous child context " + children_.back().fullTag_ + " was marked complete.");
    }
    if (!wasMarkedComplete()) {
      C8_THROW(
        String("Calling exportDetail from logging context ") + fullTag_
        + " which was expected to be marked complete but never was.");
    }
  }

  EventDetailData data;
  String eventId = std::to_string(detail.size());
  data.eventId = eventId;
  data.eventName = fullTag_;
  data.parentId = parentId;
  data.startTimeMicros = startTime_;
  data.hasEndTime = wasMarkedComplete();
  if (wasMarkedComplete()) {
    data.endTimeMicros = stopTime_;
  }
  data.count = count_;
  data.ofTotal = ofTotal_;
  detail.push_back(std::move(data));

  for (const LoggingContext &child : children_) {
    child.exportDetailInternal(eventId, detail);
  }
  for (const LoggingContext &counter : counters_) {
    counter.exportDetailInternal(eventId, detail);
  }
}

bool LoggingContext::wasMarkedComplete() const { return stopTime_ != 0; }

}  // namespace c8
