// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/stats/latency-summarizer.h"
#include "c8/stats/logging-context.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

// A ScopeTimer is an object that times its lifetime (i.e. scope).
//
// Typical usage might look like
//
// {
//   ScopeTimer t("compute-foo");
//   amazingFooAlgorightm.doCoolFooComputation();
// }
//
// Internally, this creates a new logging context on top of a thread local stack which is a child of
// the thread's current logging context, and automatically calls markCompletionTimepoint() when the
// object goes out of scope.
//
// While a ScopeTimer is in scope, it effectively becomes the new logging context. New
// counters should be added to it:
//
// {
//   ScopeTimer t("compute-foo-and-process");
//   auto fooResult = amazingFooAlgorightm.doCoolFooComputation();
//   auto fooProcessingResult = processFooWithInstrumentation(fooResult);
//   t.addCounter("foo-num-widgets", fooProcessingResult.widgetCount());
// }
//
// As an advanced feature, callers can request to "resetRoot". This has the effect of pulling a
// subtree from the current stack trace into the top level. This feature is important for keeping a
// stable representation of an inner algorithm across different outer callers. For example, the
// outer caller can be a javascript camera loop, a javascript benchmark, an android app, etc. but
// all call into the same engine code. They have different timers for the outer caller, but the
// inner timers are the same. If the timers are logged to the server, the inner timers will be the
// same across implementations.  For example:
//
// /outer                <- parent is nullptr
// /outer/a
// /outer/a/b
// /outer/a/b/inner      <- reset root
// /outer/a/b/inner/c
// /outer/a/b/inner/c/d
// /outer/a/e
//
// gets unnested to:
//
// /inner                <- parent is /outer/a/b
// /inner/c
// /inner/c/d
// /outer                <- parent is nullptr
// /outer/a
// /outer/a/b            <- Still includes timing from all of inner
// /outer/a/e
class ScopeTimer {
public:
  static ScopeTimer *current() {
    return THREAD_CURRENT_.empty() ? nullptr : THREAD_CURRENT_.back();
  }

  static LoggingContext *lastCompleted() { return LAST_COMPLETED_.get(); }
  static LatencySummarizer *summarizer() { return &SUMMARIZER_; }
  static bool hasCurrent() { return !THREAD_CURRENT_.empty(); }

  // Delegated methods, for a more concise API.
  static void reset() { SUMMARIZER_.reset(); }
  static void logBriefSummary() { SUMMARIZER_.logBriefSummary(); }
  static void logDetailedSummary() { SUMMARIZER_.logDetailedSummary(); }
  static void logFullSummary() { SUMMARIZER_.logFullSummary(); }
  static void summarize(const LoggingDetail::Reader &detail) { SUMMARIZER_.summarize(detail); }
  static void summarize(const LoggingContext &root) { SUMMARIZER_.summarize(root); }
  static void exportSummary(LoggingSummary::Builder *summaryBuilder) {
    SUMMARIZER_.exportSummary(summaryBuilder);
  };

  ScopeTimer(const String &tag, bool resetRoot) noexcept {
    if (THREAD_CURRENT_.empty() || resetRoot) {
      THREAD_CURRENT_.push_back(nullptr);
      root_ = LoggingContext::createRootLoggingTreePtr(tag);
      context_ = root_.get();
    } else {
      context_ = &THREAD_CURRENT_.back()->loggingContext()->createChild(tag);
    }
    parent_ = THREAD_CURRENT_.back();
    THREAD_CURRENT_.back() = this;
  }

  ScopeTimer(const String &tag) noexcept : ScopeTimer(tag, false) {}

  ~ScopeTimer() {
    context_->markCompletionTimepoint();
    if (parent_ == nullptr) {
      LAST_COMPLETED_ = std::move(root_);
      SUMMARIZER_.summarize(*LAST_COMPLETED_);
      THREAD_CURRENT_.pop_back();
    } else {
      THREAD_CURRENT_.back() = parent_;
    }
  }

  void addCounter(const String &tag, int count, int ofTotal = 0) {
    context_->addCounter(tag, count, ofTotal);
  }

  inline LoggingContext *loggingContext() { return context_; }

  ScopeTimer() = delete;
  ScopeTimer(ScopeTimer &&) = default;
  ScopeTimer &operator=(ScopeTimer &&) = default;
  ScopeTimer(const ScopeTimer &) = delete;
  ScopeTimer &operator=(const ScopeTimer &) = delete;

private:
  LoggingContext *context_ = nullptr;
  std::unique_ptr<LoggingContext> root_;
  ScopeTimer *parent_ = nullptr;
  // Each timer is one unnested stack including the root, e.g. if we have a tree root/a/b/c/d and
  // c is unnested, this field will contain pointers to [root, root/a/b/c]
  static thread_local Vector<ScopeTimer *> THREAD_CURRENT_;
  static thread_local std::unique_ptr<LoggingContext> LAST_COMPLETED_;
  static thread_local LatencySummarizer SUMMARIZER_;
};

}  // namespace c8
