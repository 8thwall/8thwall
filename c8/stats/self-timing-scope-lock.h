// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <memory>
#include <mutex>

#include "c8/stats/logging-context.h"
#include "c8/stats/scope-timer.h"
#include "c8/string.h"

namespace c8 {

// A ScopeTimingScopeLock is an object that acquires a lock on a mutex for its lifetime
// (i.e. scope), and times both the time to acquire the lock and the time that the lock is held.
//
// Typical usage might look like
//
// {
//   ScopeTimingScopeLock t("lock-foo", fooMutex);
//   amazingFooAlgorightm.doCoolFooComputationWhileHoldingMutexLock();
// }
//
// Internally, this creates a new logging context which is a child of the current logging context,
// and automatically calls markCompletionTimepoint() when the object goes out of scope.
//
// While a SelfTimingScopeLock is in scope, it effectively becomes the new logging context. New
// counters should be added to it, and any new subtrees that should be instrumented should refer to
// it as the new root context. Rather than use an inheritance model, we use a composition model.
// SelfTimingScopeLock exposes its own logging context via an accessor:
//
// {
//   ScopeTimingScopeLock t("lock-foo", fooMutex);
//   auto fooResult = amazingFooAlgorightm.doCoolFooComputationWhileHoldingMutexLock();
//   auto fooProcessingResult = processFooWithInstrumentation(fooResult);
//   t.loggingContext()->addCounter("foo-num-widgets", fooProcessingResult.widgetCount(), 0);
// }
class ScopeTimingScopeLock {
public:
  ScopeTimingScopeLock(const String &tag, std::mutex &mutex) noexcept {
    b_ = new ScopeTimer(tag);
    {
      ScopeTimer t1("acquire-lock");
      lock_ = new std::lock_guard<std::mutex>(mutex);
    }
    t_ = new ScopeTimer("hold-lock");
  }
  ~ScopeTimingScopeLock() {
    // Make sure to destroy objects in the correct order.
    delete lock_;
    delete t_;
    delete b_;
  }
  inline LoggingContext *loggingContext() { return t_->loggingContext(); }

  // SelfTimingScopeLocks cannot be default constructed, copied or moved.
  ScopeTimingScopeLock() = delete;
  ScopeTimingScopeLock(ScopeTimingScopeLock &&) = delete;
  ScopeTimingScopeLock &operator=(ScopeTimingScopeLock &&) = delete;
  ScopeTimingScopeLock(const ScopeTimingScopeLock &) = delete;
  ScopeTimingScopeLock &operator=(const ScopeTimingScopeLock &) = delete;

private:
  std::lock_guard<std::mutex> *lock_;
  ScopeTimer *t_;
  ScopeTimer *b_;
};

}  // namespace c8
