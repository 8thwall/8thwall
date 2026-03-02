// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "scope-timer.h",
  };
  deps = {
    ":latency-summarizer",
    ":logging-context",
    "//c8:string",
    "//c8:vector",
  };
}
cc_end(0xf6561b77);

#include "c8/stats/scope-timer.h"

namespace c8 {

thread_local Vector<ScopeTimer*> ScopeTimer::THREAD_CURRENT_;
thread_local std::unique_ptr<LoggingContext> ScopeTimer::LAST_COMPLETED_;
thread_local LatencySummarizer ScopeTimer::SUMMARIZER_;

}  // namespace c8
